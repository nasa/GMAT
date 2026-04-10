//$Id$
//------------------------------------------------------------------------------
//                            ParameterFactory
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed further jointly by NASA/GSFC, Thinking Systems, Inc., and 
// Schafer Corp., under AFRL NOVA Contract #FA945104D03990003
//
// Author: Darrel Conway
// Created: 2003/10/28
// Modified:  Dunn Idle (added MRPs)
// Date:      2010/08/24
//
/**
 *  Implementation code for the ParameterFactory class, responsible
 *  for creating Parameter objects.
 */
//------------------------------------------------------------------------------


#include "ParameterFactory.hpp"

#include "../parameter/OrbitTime.hpp"
#include "TimeParameters.hpp"
#include "CartesianParameters.hpp"
#include "KeplerianParameters.hpp"
#include "SphericalParameters.hpp"
#include "EquinoctialParameters.hpp"
#include "ModEquinoctialParameters.hpp"
#include "AlternateEquinoctialParameters.hpp"
#include "DelaunayParameters.hpp"
#include "PlanetodeticParameters.hpp"
#include "IncomingAsymptoteParameters.hpp"
#include "OutgoingAsymptoteParameters.hpp"
#include "BrouwerMeanShortParameters.hpp"
#include "BrouwerMeanLongParameters.hpp"

#include "OrbitalParameters.hpp"
#include "AngularParameters.hpp"
#include "PlanetParameters.hpp"
#include "Variable.hpp"
#include "StringVar.hpp"
#include "Array.hpp"
#include "BplaneParameters.hpp"
#include "BurnParameters.hpp"
#include "AttitudeParameters.hpp"
#include "BallisticMassParameters.hpp"
#include "OrbitStmParameters.hpp"
#include "OrbitCovarianceParameters.hpp"
#include "HardwareParameters.hpp"
#include "MessageInterface.hpp"


//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  GmatBase* CreateParameter(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Parameter class
 *
 * @param <ofType> the Parameter object to create and return.
 * @param <withName> the name to give the newly-created Parameter object.
 */
//------------------------------------------------------------------------------
GmatBase* ParameterFactory::CreateObject(const std::string &ofType,
                                         const std::string &withName)
{
   return CreateParameter(ofType, withName);
}

//------------------------------------------------------------------------------
//  CreateParameter(const std::string &ofType, const std::string &withName)
//------------------------------------------------------------------------------
/**
 * This method creates and returns an object of the requested Parameter class
 *
 * @param <ofType> the Parameter object to create and return.
 * @param <withName> the name to give the newly-created Parameter object.
 */
//------------------------------------------------------------------------------
Parameter* ParameterFactory::CreateParameter(const std::string &ofType,
                                             const std::string &withName)
{
   #ifdef DEBUG_CREATE_PARAM
   MessageInterface::ShowMessage
      ("ParameterFactory::CreateParameter() entered, type = '%s', name = '%s'\n",
       ofType.c_str(), withName.c_str());
   #endif
   
   // User defined parameters
   if (ofType == "Variable")
      return new Variable(withName);
   if (ofType == "String")
      return new StringVar(withName);
   if (ofType == "Array")
      return new Array(withName);
   
   // Time parameters
   if (ofType == "ElapsedDays")
      return new ElapsedDays(withName);
   if (ofType == "ElapsedSecs")
      return new ElapsedSecs(withName);
   if (ofType == "CurrA1MJD")            // deprecated
      return new CurrA1MJD(withName);
   if (ofType == "A1ModJulian")
      return new A1ModJulian(withName);
   if (ofType == "A1Gregorian")
      return new A1Gregorian(withName);
   if (ofType == "TAIModJulian")
      return new TAIModJulian(withName);
   if (ofType == "TAIGregorian")
      return new TAIGregorian(withName);
   if (ofType == "TTModJulian")
      return new TTModJulian(withName);
   if (ofType == "TTGregorian")
      return new TTGregorian(withName);
   if (ofType == "TDBModJulian")
      return new TDBModJulian(withName);
   if (ofType == "TDBGregorian")
      return new TDBGregorian(withName);
   if (ofType == "UTCModJulian")
      return new UTCModJulian(withName);
   if (ofType == "UTCGregorian")
      return new UTCGregorian(withName);

   // Cartesian parameters
   if (ofType == "X")
      return new CartX(withName);
   if (ofType == "Y")
      return new CartY(withName);
   if (ofType == "Z")
      return new CartZ(withName);
   if (ofType == "VX")
      return new CartVx(withName);
   if (ofType == "VY")
      return new CartVy(withName);
   if (ofType == "VZ")
      return new CartVz(withName);
   if (ofType == "Cartesian")
      return new CartState(withName);

    // Keplerian parameters
   if (ofType == "SMA")
      return new KepSMA(withName);
   if (ofType == "ECC")
      return new KepEcc(withName);
   if (ofType == "INC")
      return new KepInc(withName);
   if (ofType == "RAAN")
      return new KepRAAN(withName);
   if (ofType == "RADN")
      return new KepRADN(withName);
   if (ofType == "AOP")
      return new KepAOP(withName);
   if (ofType == "TA")
      return new KepTA(withName);
   if (ofType == "MA")
      return new KepMA(withName);
   if (ofType == "EA")
      return new KepEA(withName);
   if (ofType == "HA")
      return new KepHA(withName);
   if (ofType == "MM")
      return new KepMM(withName);
   if (ofType == "Keplerian")
      return new KepElem(withName);
   if (ofType == "ModKeplerian")
      return new ModKepElem(withName);

   // Spherical parameters
   if (ofType == "RMAG")
      return new SphRMag(withName);
   if (ofType == "RA")
      return new SphRA(withName);
   if (ofType == "DEC")
      return new SphDec(withName);
   if (ofType == "VMAG")
      return new SphVMag(withName);
   if (ofType == "RAV")
      return new SphRAV(withName);
   if (ofType == "DECV")
      return new SphDecV(withName);
   if (ofType == "AZI")
      return new SphAzi(withName);
   if (ofType == "FPA")
      return new SphFPA(withName);
   if (ofType == "SphericalRADEC") 
      return new SphRaDecElem(withName);
   if (ofType == "SphericalAZFPA") 
      return new SphAzFpaElem(withName);
   if (ofType == "Altitude")
      return new Altitude(withName);

   // Equinoctial parameters
   if (ofType == "EquinoctialH")
      return new EquinEy(withName);
   if (ofType == "EquinoctialK")
      return new EquinEx(withName);
   if (ofType == "EquinoctialP")
      return new EquinNy(withName);
   if (ofType == "EquinoctialQ")
      return new EquinNx(withName);
   if (ofType == "MLONG")
      return new EquinMlong(withName);
   if (ofType == "Equinoctial")
      return new EquinState(withName);
   
   // Equinoctial rate parameters
   if (ofType == "SMADot")
      return new SMADot(withName);
   if (ofType == "EquinoctialHDot")
      return new EquinEyDot(withName);
   if (ofType == "EquinoctialKDot")
      return new EquinExDot(withName);
   if (ofType == "EquinoctialPDot")
      return new EquinNyDot(withName);
   if (ofType == "EquinoctialQDot")
      return new EquinNxDot(withName);
   if (ofType == "TLONGDot")
      return new EquinTlongDot(withName);

   // ModifiedEquinoctial parameters; Modified by M.H.
   // Changed SemiLatusRectum to SemilatusRectum (Fix for GMT-4173)
   // It will create SemilatusRectum Parameter due to issues (2014.01.28)
   //if (ofType == "SemiLatusRectum")
   //   return new ModEquinP(withName);
   if (ofType == "ModEquinoctialF")
      return new ModEquinF(withName);
   if (ofType == "ModEquinoctialG")
      return new ModEquinG(withName);
   if (ofType == "ModEquinoctialH")
      return new ModEquinH(withName);
   if (ofType == "ModEquinoctialK")
      return new ModEquinK(withName);
   if (ofType == "TLONG")
      return new ModEquinTLONG(withName);
   if (ofType == "ModifiedEquinoctial")
      return new ModEquinState(withName);
   
   // Alternate Equinoctial parameters by HYKim
   if (ofType == "AltEquinoctialP")
      return new AltEquinP(withName);
   if (ofType == "AltEquinoctialQ")
      return new AltEquinQ(withName);
   if (ofType == "AltEquinoctial")
      return new AltEquinState(withName);
   
   // Delaunay parameters; Modified by M.H.
   if (ofType == "Delaunayl")
      return new Dela_l(withName);
   if (ofType == "Delaunayg")
      return new Dela_g(withName);
   if (ofType == "Delaunayh")
      return new Dela_h(withName);
   if (ofType == "DelaunayL")
      return new DelaL(withName);
   if (ofType == "DelaunayG")
      return new DelaG(withName);
   if (ofType == "DelaunayH")
      return new DelaH(withName);
   if (ofType == "Delaunay")
      return new DelaState(withName);

   // Planetodetic parameters; Modified by M.H.
   if (ofType == "PlanetodeticRMAG")
      return new PldRMAG(withName);
   if (ofType == "PlanetodeticLON")
      return new PldLON(withName);
   if (ofType == "PlanetodeticLAT")
      return new PldLAT(withName);
   if (ofType == "PlanetodeticVMAG")
      return new PldVMAG(withName);
   if (ofType == "PlanetodeticAZI")
      return new PldAZI(withName);
   if (ofType == "PlanetodeticHFPA")
      return new PldHFPA(withName);
   if (ofType == "Planetodetic")
      return new PldState(withName);
   
   // IncomingAsymptote parameters; Modified by YK
   // Added HyperbolicRadPer (LOJ: 2014.04.28)
   //if (ofType == "HyperbolicRadPer")
   //   return new HyperbolicRadPer(withName);
   // Added IncomingRadPer and IncomingC3Energy (LOJ: 2014.05.07)
   if (ofType == "IncomingRadPer")
      return new IncAsymRadPer(withName);
   if (ofType == "IncomingC3Energy")
      return new IncAsymC3Energy(withName);
   if (ofType == "IncomingRHA")
      return new IncAsymRHA(withName);
   if (ofType == "IncomingDHA")
      return new IncAsymDHA(withName);
   if (ofType == "IncomingBVAZI")
      return new IncAsymBVAZI(withName);
   
   // OutgoingAsymptote parameters; Modified by YK
   // Added OutgoingRadPer and OutgoingC3Energy (LOJ: 2014.05.07)
   if (ofType == "OutgoingRadPer")
      return new OutAsymRadPer(withName);
   if (ofType == "OutgoingC3Energy")
      return new OutAsymC3Energy(withName);
   if (ofType == "OutgoingRHA")
      return new OutAsymRHA(withName);
   if (ofType == "OutgoingDHA")
      return new OutAsymDHA(withName);
   if (ofType == "OutgoingBVAZI")
      return new OutAsymBVAZI(withName);
   
   // Brouwer Mean Short parameters; Modified by YK
   // Changed Parameter names (see GMT-4228 LOJ: 2014.01.09)
   if (ofType == "BrouwerShortSMA")
      return new BLshortSMAP(withName);
   if (ofType == "BrouwerShortECC")
      return new BLshortECCP(withName);
   if (ofType == "BrouwerShortINC")
      return new BLshortINCP(withName);
   if (ofType == "BrouwerShortRAAN")
      return new BLshortRAANP(withName);
   if (ofType == "BrouwerShortAOP")
      return new BLshortAOPP(withName);
   if (ofType == "BrouwerShortMA")
      return new BLshortMAP(withName);
   
   // BrouwerMeanLong parameters; Modified by YK
   // Changed Parameter names (see GMT-4228 LOJ: 2014.01.09)
   if (ofType == "BrouwerLongSMA")
      return new BLlongSMADP(withName);
   if (ofType == "BrouwerLongECC")
      return new BLlongECCDP(withName);
   if (ofType == "BrouwerLongINC")
      return new BLlongINCDP(withName);
   if (ofType == "BrouwerLongRAAN")
      return new BLlongRAANDP(withName);
   if (ofType == "BrouwerLongAOP")
      return new BLlongAOPDP(withName);
   if (ofType == "BrouwerLongMA")
      return new BLlongMADP(withName);
   
   // Orbital parameters
   if (ofType == "VelApoapsis")
      return new VelApoapsis(withName);
   if (ofType == "VelPeriapsis")
      return new VelPeriapsis(withName);
   if (ofType == "Apoapsis")
      return new Apoapsis(withName);
   if (ofType == "Periapsis")
      return new Periapsis(withName);
   if (ofType == "OrbitPeriod")
      return new OrbitPeriod(withName);
   if (ofType == "RadApo")
      return new ModKepRadApo(withName);
   if (ofType == "RadPer")
      return new ModKepRadPer(withName);
   if (ofType == "C3Energy")
      return new C3Energy(withName);
   if (ofType == "Energy")
      return new Energy(withName);

   // Angular parameters
   // Changed to create ModEquinP() since it is settable Parameter
   // Fix for GMT-4173 (LOJ: 2014.01.22)
   // Changed back to create SemilatusRectum due to issues (LOJ: 2014.01.28)
   if (ofType == "SemilatusRectum")
      return new SemilatusRectum(withName);
   if (ofType == "HMAG")
      return new AngularMomentumMag(withName);
   if (ofType == "HX")
      return new AngularMomentumX(withName);
   if (ofType == "HY")
      return new AngularMomentumY(withName);
   if (ofType == "HZ")
      return new AngularMomentumZ(withName);
   if (ofType == "DLA")
      return new DLA(withName);
   if (ofType == "RLA")
      return new RLA(withName);
   
   // Planet parameters
   if (ofType == "MHA")
      return new MHA(withName);
   if (ofType == "Longitude")
      return new Longitude(withName);
   if (ofType == "Latitude")
      return new Latitude(withName);
   if (ofType == "LST")
      return new LST(withName);
   if (ofType == "BetaAngle")
      return new BetaAngle(withName);
   
   // B-Plane parameters
   if (ofType == "BdotT")
      return new BdotT(withName);
   if (ofType == "BdotR")
      return new BdotR(withName);
   if (ofType == "BVectorMag")
      return new BVectorMag(withName);
   if (ofType == "BVectorAngle")
      return new BVectorAngle(withName);
   
   // ImpulsiveBurn parameters
   if (ofType == "Element1" || ofType == "Element2" || ofType == "Element3")
      return new ImpBurnElements(ofType, withName);   
   if (ofType == "V" || ofType == "N" || ofType == "B")
      return new ImpBurnElements(ofType, withName);
   
   // FiniteBurn parameters
   if (ofType == "TotalMassFlowRate")
      return new TotalMassFlowRate(ofType, withName);
   if (ofType == "TotalAcceleration1" || ofType == "TotalAcceleration2" || ofType == "TotalAcceleration3")
      return new TotalAcceleration(ofType, withName);
   if (ofType == "TotalThrust1" || ofType == "TotalThrust2" || ofType == "TotalThrust3")
      return new TotalThrust(ofType, withName);
   
   // Attitude parameters
   if (ofType == "DCM11" || ofType == "DirectionCosineMatrix11")
      return new DCM11(withName);
   if (ofType == "DCM12" || ofType == "DirectionCosineMatrix12")
      return new DCM12(withName);
   if (ofType == "DCM13" || ofType == "DirectionCosineMatrix13")
      return new DCM13(withName);
   if (ofType == "DCM21" || ofType == "DirectionCosineMatrix21")
      return new DCM21(withName);
   if (ofType == "DCM22" || ofType == "DirectionCosineMatrix22")
      return new DCM22(withName);
   if (ofType == "DCM23" || ofType == "DirectionCosineMatrix23")
      return new DCM23(withName);
   if (ofType == "DCM31" || ofType == "DirectionCosineMatrix31")
      return new DCM31(withName);
   if (ofType == "DCM32" || ofType == "DirectionCosineMatrix32")
      return new DCM32(withName);
   if (ofType == "DCM33" || ofType == "DirectionCosineMatrix33")
      return new DCM33(withName);
   if (ofType == "DirectionCosineMatrix")
      return new DirectionCosineMatrix(withName);

   if (ofType == "EulerAngle1")
      return new EulerAngle1(withName);
   if (ofType == "EulerAngle2")
      return new EulerAngle2(withName);
   if (ofType == "EulerAngle3")
      return new EulerAngle3(withName);
   if (ofType == "MRP1")  // Dunn Added
      return new MRP1(withName);
   if (ofType == "MRP2")  // Dunn Added
      return new MRP2(withName);
   if (ofType == "MRP3")  // Dunn Added
      return new MRP3(withName);
   if (ofType == "Q1" || ofType == "q1")
      return new Quat1(withName);
   if (ofType == "Q2" || ofType == "q2")
      return new Quat2(withName);
   if (ofType == "Q3" || ofType == "q3")
      return new Quat3(withName);
   if (ofType == "Q4" || ofType == "q4")
      return new Quat4(withName);
   if (ofType == "Quaternion")
      return new Quaternion(withName);
   
   if (ofType == "AngularVelocityX")
      return new AngularVelocityX(withName);
   if (ofType == "AngularVelocityY")
      return new AngularVelocityY(withName);
   if (ofType == "AngularVelocityZ")
      return new AngularVelocityZ(withName);
   if (ofType == "EulerAngleRate1")
      return new EulerAngleRate1(withName);
   if (ofType == "EulerAngleRate2")
      return new EulerAngleRate2(withName);
   if (ofType == "EulerAngleRate3")
      return new EulerAngleRate3(withName);
   
   if (ofType == "BodyAlignmentVectorX") 
      return new BodyAlignmentVectorX(withName);
   if (ofType == "BodyAlignmentVectorY") 
      return new BodyAlignmentVectorY(withName);
   if (ofType == "BodyAlignmentVectorZ")
      return new BodyAlignmentVectorZ(withName);
   if (ofType == "BodyConstraintVectorX")
      return new BodyConstraintVectorX(withName);
   if (ofType == "BodyConstraintVectorY")
      return new BodyConstraintVectorY(withName);
   if (ofType == "BodyConstraintVectorZ")
      return new BodyConstraintVectorZ(withName);

   if (ofType == "Attitude")             
      return new AttitudeParam(withName);
   if (ofType == "AttitudeReferenceBody")
      return new AttitudeReferenceBody(withName);
   if (ofType == "AttitudeConstraintType")
      return new AttitudeConstraintType(withName);

   // Ballistic/Mass parameters
   if (ofType == "DryMass")
      return new DryMass(withName);
   if (ofType == "DryCenterOfMassX")
      return new DryCenterOfMassX(withName);
   if (ofType == "DryCenterOfMassY")
      return new DryCenterOfMassY(withName);
   if (ofType == "DryCenterOfMassZ")
      return new DryCenterOfMassZ(withName);
   if (ofType == "DryMomentOfInertiaXX")
      return new DryMomentOfInertiaXX(withName);
   if (ofType == "DryMomentOfInertiaXY")
      return new DryMomentOfInertiaXY(withName);
   if (ofType == "DryMomentOfInertiaXZ")
      return new DryMomentOfInertiaXZ(withName);
   if (ofType == "DryMomentOfInertiaYY")
      return new DryMomentOfInertiaYY(withName);
   if (ofType == "DryMomentOfInertiaYZ")
      return new DryMomentOfInertiaYZ(withName);
   if (ofType == "DryMomentOfInertiaZZ")
      return new DryMomentOfInertiaZZ(withName);
   if (ofType == "Cd")
      return new DragCoeff(withName);
   if (ofType == "Cr")
      return new ReflectCoeff(withName);
   if (ofType == "DragArea")
      return new DragArea(withName);
   if (ofType == "SRPArea")
      return new SRPArea(withName);
   if (ofType == "TotalMass")
      return new TotalMass(withName);
   if (ofType == "SystemCenterOfMassX")
      return new SystemCenterOfMassX(withName);
   if (ofType == "SystemCenterOfMassY")
      return new SystemCenterOfMassY(withName);
   if (ofType == "SystemCenterOfMassZ")
      return new SystemCenterOfMassZ(withName);
   if (ofType == "SystemMomentOfInertiaXX")
      return new SystemMomentOfInertiaXX(withName);
   if (ofType == "SystemMomentOfInertiaXY")
      return new SystemMomentOfInertiaXY(withName);
   if (ofType == "SystemMomentOfInertiaXZ")
      return new SystemMomentOfInertiaXZ(withName);
   if (ofType == "SystemMomentOfInertiaYY")
      return new SystemMomentOfInertiaYY(withName);
   if (ofType == "SystemMomentOfInertiaYZ")
      return new SystemMomentOfInertiaYZ(withName);
   if (ofType == "SystemMomentOfInertiaZZ")
      return new SystemMomentOfInertiaZZ(withName);
   if (ofType == "SPADDragScaleFactor")
      return new SpadDragScaleFactor(withName);
   if (ofType == "SPADSRPScaleFactor")
      return new SpadSRPScaleFactor(withName);
   if (ofType == "AtmosDensityScaleFactor")
      return new AtmosDensityScaleFactor(withName);
   if (ofType == "CdSigma")
      return new DragCoeffSigma(withName);
   if (ofType == "CrSigma")
      return new ReflectCoeffSigma(withName);
   if (ofType == "SPADDragScaleFactorSigma")
      return new SpadDragScaleFactorSigma(withName);
   if (ofType == "SPADSRPScaleFactorSigma")
      return new SpadSRPScaleFactorSigma(withName);
   if (ofType == "AtmosDensityScaleFactorSigma")
      return new AtmosDensityScaleFactorSigma(withName);
   
   // orbit STM parameters
   if (ofType == "OrbitSTM")
      return new OrbitStm(withName);
   if (ofType == "OrbitSTMA")
      return new OrbitStmA(withName);
   if (ofType == "OrbitSTMB")
      return new OrbitStmB(withName);
   if (ofType == "OrbitSTMC")
      return new OrbitStmC(withName);
   if (ofType == "OrbitSTMD")
      return new OrbitStmD(withName);

   // orbit Covariance parameters
   if (ofType == "OrbitErrorCovariance")
      return new OrbitErrorCovariance(withName);
   
   // FuelTank parameters
   if (ofType == "FuelMass")
      return new FuelMass(withName);
   if (ofType == "FuelCenterOfMassX")
      return new FuelCenterOfMassX(withName);
   if (ofType == "FuelCenterOfMassY")
      return new FuelCenterOfMassY(withName);
   if (ofType == "FuelCenterOfMassZ")
      return new FuelCenterOfMassZ(withName);
   if (ofType == "FuelMomentOfInertiaXX")
      return new FuelMomentOfInertiaXX(withName);
   if (ofType == "FuelMomentOfInertiaXY")
      return new FuelMomentOfInertiaXY(withName);
   if (ofType == "FuelMomentOfInertiaXZ")
      return new FuelMomentOfInertiaXZ(withName);
   if (ofType == "FuelMomentOfInertiaYY")
      return new FuelMomentOfInertiaYY(withName);
   if (ofType == "FuelMomentOfInertiaYZ")
      return new FuelMomentOfInertiaYZ(withName);
   if (ofType == "FuelMomentOfInertiaZZ")
      return new FuelMomentOfInertiaZZ(withName);
   if (ofType == "FuelCenterOfMassX_BCS")
      return new FuelCenterOfMassX_BCS(withName);
   if (ofType == "FuelCenterOfMassY_BCS")
      return new FuelCenterOfMassY_BCS(withName);
   if (ofType == "FuelCenterOfMassZ_BCS")
      return new FuelCenterOfMassZ_BCS(withName);
   if (ofType == "FuelMomentOfInertiaXX_BCS")
      return new FuelMomentOfInertiaXX_BCS(withName);
   if (ofType == "FuelMomentOfInertiaXY_BCS")
      return new FuelMomentOfInertiaXY_BCS(withName);
   if (ofType == "FuelMomentOfInertiaXZ_BCS")
      return new FuelMomentOfInertiaXZ_BCS(withName);
   if (ofType == "FuelMomentOfInertiaYY_BCS")
      return new FuelMomentOfInertiaYY_BCS(withName);
   if (ofType == "FuelMomentOfInertiaYZ_BCS")
      return new FuelMomentOfInertiaYZ_BCS(withName);
   if (ofType == "FuelMomentOfInertiaZZ_BCS")
      return new FuelMomentOfInertiaZZ_BCS(withName);
   if (ofType == "Pressure")
      return new Pressure(withName);
   if (ofType == "Temperature")
      return new Temperature(withName);
   if (ofType == "RefTemperature")
      return new RefTemperature(withName);
   if (ofType == "Volume")
      return new Volume(withName);
   if (ofType == "FuelDensity")
      return new FuelDensity(withName);
   
   // Hardware parameters
   if (ofType == "R_SB11")
      return new R_SB11(withName);
   if (ofType == "R_SB12")
      return new R_SB12(withName);
   if (ofType == "R_SB13")
      return new R_SB13(withName);
   if (ofType == "R_SB21")
      return new R_SB21(withName);
   if (ofType == "R_SB22")
      return new R_SB22(withName);
   if (ofType == "R_SB23")
      return new R_SB23(withName);
   if (ofType == "R_SB31")
      return new R_SB31(withName);
   if (ofType == "R_SB32")
      return new R_SB32(withName);
   if (ofType == "R_SB33")
      return new R_SB33(withName);

   // Thruster parameters
   if (ofType == "DutyCycle")
      return new DutyCycle(withName);
   if (ofType == "ThrustScaleFactor")
      return new ThrustScaleFactor(withName);
   if (ofType == "GravitationalAccel")
      return new GravitationalAccel(withName);
   if (ofType == "ThrustMagnitude")
      return new ThrustMagnitude(withName);
   if (ofType == "Isp")
      return new Isp(withName);
   if (ofType == "MassFlowRate")
      return new MassFlowRate(withName);

   if (ofType == "C1"  || ofType == "C2"  || ofType == "C3"  || ofType == "C4"  ||
       ofType == "C5"  || ofType == "C6"  || ofType == "C7"  || ofType == "C8"  ||
       ofType == "C9"  || ofType == "C10" || ofType == "C11" || ofType == "C12" ||
       ofType == "C13" || ofType == "C14" || ofType == "C15" || ofType == "C16")
      return new ThrustCoefficients(ofType, withName);
   
   if (ofType == "K1"  || ofType == "K2"  || ofType == "K3"  || ofType == "K4"  ||
       ofType == "K5"  || ofType == "K6"  || ofType == "K7"  || ofType == "K8"  ||
       ofType == "K9"  || ofType == "K10" || ofType == "K11" || ofType == "K12" ||
       ofType == "K13" || ofType == "K14" || ofType == "K15" || ofType == "K16")
      return new ImpulseCoefficients(ofType, withName);
   
   if (ofType == "ThrustDirection1" || ofType == "ThrustDirection2" ||
       ofType == "ThrustDirection3")
      return new ThrustDirections(ofType, withName);
   
   //Plate parameters 
   if (ofType == "AreaCoefficient")
      return new AreaCoefficient(withName);
   if (ofType == "SpecularFraction")
      return new SpecularFraction(withName);
   if (ofType == "DiffuseFraction")
      return new DiffuseFraction(withName);
   if (ofType == "Area")
      return new Area(withName);
   //if (ofType == "Type")
      //return new Type(withName);
   if (ofType == "LitFraction")
      return new LitFraction(withName);
   if (ofType == "PlateNormal")
      return new PlateNormal(withName);
   if (ofType == "PlateNormalHistoryFile")
      return new PlateNormalHistoryFile(withName);
   if (ofType == "AreaCoefficientSigma")
      return new AreaCoefficientSigma(withName);
   if (ofType == "SpecularFractionSigma")
      return new SpecularFractionSigma(withName);
   if (ofType == "DiffuseFractionSigma")
      return new DiffuseFractionSigma(withName);
   if (ofType == "PlateX")
      return new PlateX(withName);
   if (ofType == "PlateY")
      return new PlateY(withName);
   if (ofType == "PlateZ")
      return new PlateZ(withName);


   // PowerSystem parameters
   if (ofType == "TotalPowerAvailable")
      return new TotalPowerAvailable(withName);
   if (ofType == "RequiredBusPower")
      return new RequiredBusPower(withName);
   if (ofType == "ThrustPowerAvailable")
      return new ThrustPowerAvailable(withName);

   // OrbitTime and Midnight
   if (ofType == "OrbitTime")
      return new OrbitTime(withName);

   // add others here
   
   MessageInterface::ShowMessage
      ("**** ERROR **** Cannot create a parameter with unknown type \"%s\"\n",
       ofType.c_str());
   
   return NULL;
}


//------------------------------------------------------------------------------
//  ParameterFactory()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ParameterFactory
 * (default constructor).
 *
 *
 */
//------------------------------------------------------------------------------
ParameterFactory::ParameterFactory()
   : Factory(Gmat::PARAMETER)
{
   if (creatables.empty())
   {
      // User defined parameters
      creatables.push_back("Variable");
      creatables.push_back("String");
      creatables.push_back("Array");
      
      // Time parameters
      creatables.push_back("ElapsedDays");
      creatables.push_back("ElapsedSecs");
      creatables.push_back("CurrA1MJD");     // deprecated
      creatables.push_back("A1ModJulian");
      creatables.push_back("A1Gregorian");
      creatables.push_back("TAIModJulian");
      creatables.push_back("TAIGregorian");
      creatables.push_back("TTModJulian");
      creatables.push_back("TTGregorian");
      creatables.push_back("TDBModJulian");
      creatables.push_back("TDBGregorian");
      creatables.push_back("UTCModJulian");
      creatables.push_back("UTCGregorian");

      // Cartesian parameters
      creatables.push_back("X");
      creatables.push_back("Y");
      creatables.push_back("Z");
      creatables.push_back("VX");
      creatables.push_back("VY");
      creatables.push_back("VZ");
      creatables.push_back("Cartesian");

      // Keplerian parameters
      creatables.push_back("SMA");
      creatables.push_back("ECC");
      creatables.push_back("INC");
      creatables.push_back("RAAN");
      creatables.push_back("RADN");
      creatables.push_back("AOP");
      creatables.push_back("TA");
      creatables.push_back("MA");
      creatables.push_back("EA");
      creatables.push_back("HA");
      creatables.push_back("MM");
      creatables.push_back("Keplerian");
      creatables.push_back("RadApo");
      creatables.push_back("RadPer");
      creatables.push_back("ModKeplerian");

      // Spherical parameters
      creatables.push_back("RMAG");
      creatables.push_back("RA");
      creatables.push_back("DEC");
      creatables.push_back("VMAG");
      creatables.push_back("RAV");
      creatables.push_back("DECV");
      creatables.push_back("AZI");
      creatables.push_back("FPA");
      creatables.push_back("SphericalRADEC");
      creatables.push_back("SphericalAZFPA");
      creatables.push_back("Altitude");

      // Equinoctial parameters
      creatables.push_back("EquinoctialH");
      creatables.push_back("EquinoctialK");
      creatables.push_back("EquinoctialP");
      creatables.push_back("EquinoctialQ");
      creatables.push_back("MLONG");
      creatables.push_back("Equinoctial");
      
      // Equinoctial rate parameters
      creatables.push_back("SMADot");
      creatables.push_back("EquinoctialHDot");
      creatables.push_back("EquinoctialKDot");
      creatables.push_back("EquinoctialPDot");
      creatables.push_back("EquinoctialQDot");
      creatables.push_back("TLONGDot");

      // ModifedEquinoctial parameters ; Modified by M.H.
      // Changed SemiLatusRectum to SemilatusRectum (Fix for GMT-4173 (LOJ: 2014.01.22))
      creatables.push_back("SemilatusRectum");
      creatables.push_back("ModEquinoctialF");
      creatables.push_back("ModEquinoctialG");
      creatables.push_back("ModEquinoctialH");
      creatables.push_back("ModEquinoctialK");
      creatables.push_back("TLONG");
      creatables.push_back("ModEquinoctial");
      
      // Alternate Equinoctial parameters by HYKim
      creatables.push_back("AltEquinoctialP");
      creatables.push_back("AltEquinoctialQ");
      
      // Delaunay parameters ; Modified by M.H.
      creatables.push_back("Delaunayl");
      creatables.push_back("Delaunayg");
      creatables.push_back("Delaunayh");
      creatables.push_back("DelaunayL");
      creatables.push_back("DelaunayG");
      creatables.push_back("DelaunayH");
      creatables.push_back("Delaunay");
      
      // Planetodetic parameters ; Modified by M.H.
      creatables.push_back("PlanetodeticRMAG");
      creatables.push_back("PlanetodeticLON");
      creatables.push_back("PlanetodeticLAT");
      creatables.push_back("PlanetodeticVMAG");
      creatables.push_back("PlanetodeticAZI");
      creatables.push_back("PlanetodeticHFPA");
      creatables.push_back("Planetodetic");

      // IncomingAsymptote parameters ; Modified by YK
      //creatables.push_back("HyperbolicRadPer");
      creatables.push_back("IncomingRadPer");
      creatables.push_back("IncomingC3Energy");
      creatables.push_back("IncomingRHA");
      creatables.push_back("IncomingDHA");
      creatables.push_back("IncomingBVAZI");
      creatables.push_back("IncomingAsymptote");
      
      // OutgoingAsymptote parameters ; Modified by YK
      creatables.push_back("OutgoingRadPer");
      creatables.push_back("OutgoingC3Energy");
      creatables.push_back("OutgoingRHA");
      creatables.push_back("OutgoingDHA");
      creatables.push_back("OutgoingBVAZI");
      creatables.push_back("OutgoingAsymptote");
      
      // BrourMeanShort parameters ; Modified by YK
      // Changed Parameter names (see GMT-4228 LOJ: 2014.01.09)
      creatables.push_back("BrouwerShortSMA");
      creatables.push_back("BrouwerShortECC");
      creatables.push_back("BrouwerShortINC");
      creatables.push_back("BrouwerShortRAAN");
      creatables.push_back("BrouwerShortAOP");
      creatables.push_back("BrouwerShortMA");
      creatables.push_back("BrouwerMeanShort");
      
      // BrouwerMeanLong parameters ; Modified by YK
      // Changed Parameter names (see GMT-4228 LOJ: 2014.01.09)
      creatables.push_back("BrouwerLongSMA");
      creatables.push_back("BrouwerLongECC");
      creatables.push_back("BrouwerLongINC");
      creatables.push_back("BrouwerLongRAAN");
      creatables.push_back("BrouwerLongAOP");
      creatables.push_back("BrouwerLongMA");
      creatables.push_back("BrouwerMeanLong");
      
      // Orbital parameters
      creatables.push_back("VelApoapsis");
      creatables.push_back("VelPeriapsis");
      creatables.push_back("Apoapsis");
      creatables.push_back("Periapsis");
      creatables.push_back("OrbitPeriod");
      creatables.push_back("C3Energy");
      creatables.push_back("Energy");
      
      // Angular parameters
      // Changed SemiLatusRectum to SemilatusRectum above
      // and commented out this (LOJ: 2013.01.22)
      //creatables.push_back("SemilatusRectum");
      creatables.push_back("HMAG");
      creatables.push_back("HX");
      creatables.push_back("HY");
      creatables.push_back("HZ");
      creatables.push_back("DLA");
      creatables.push_back("RLA");
      
      // Environmental parameters
      #ifdef __ENABLE_ATMOS_DENSITY__
      creatables.push_back("AtmosDensity");
      #endif
      
      // Planet parameters
      creatables.push_back("MHA");
      creatables.push_back("Longitude");
      creatables.push_back("Latitude");
      creatables.push_back("LST");
      creatables.push_back("BetaAngle");
      
      // B-Plane parameters
      creatables.push_back("BdotT");
      creatables.push_back("BdotR");
      creatables.push_back("BVectorMag");
      creatables.push_back("BVectorAngle");
      
      // Burn parameters
      creatables.push_back("Element1");
      creatables.push_back("Element2");
      creatables.push_back("Element3");
      creatables.push_back("V");
      creatables.push_back("N");
      creatables.push_back("B");
      creatables.push_back("TotalMassFlowRate");
      creatables.push_back("TotalAcceleration1");
      creatables.push_back("TotalAcceleration2");
      creatables.push_back("TotalAcceleration3");
      creatables.push_back("TotalThrust1");
      creatables.push_back("TotalThrust2");
      creatables.push_back("TotalThrust3");
      
      // Attitude parameters
      creatables.push_back("DCM11");
      creatables.push_back("DCM12");
      creatables.push_back("DCM13");
      creatables.push_back("DCM21");
      creatables.push_back("DCM22");
      creatables.push_back("DCM23");
      creatables.push_back("DCM31");
      creatables.push_back("DCM32");
      creatables.push_back("DCM33");
      creatables.push_back("DirectionCosineMatrix");
      creatables.push_back("EulerAngle1");
      creatables.push_back("EulerAngle2");
      creatables.push_back("EulerAngle3");
      creatables.push_back("MRP1");  // Dunn Added
      creatables.push_back("MRP2");  // Dunn Added
      creatables.push_back("MRP3");  // Dunn Added
      creatables.push_back("Q1");
      creatables.push_back("Q2");
      creatables.push_back("Q3");
      creatables.push_back("Q4");
      creatables.push_back("Quaternion");
      creatables.push_back("AngularVelocityX");
      creatables.push_back("AngularVelocityY");
      creatables.push_back("AngularVelocityZ");
      creatables.push_back("EulerAngleRate1");
      creatables.push_back("EulerAngleRate2");
      creatables.push_back("EulerAngleRate3");
      creatables.push_back("BodyAlignmentVectorX");
      creatables.push_back("BodyAlignmentVectorY");
      creatables.push_back("BodyAlignmentVectorZ");
      creatables.push_back("BodyConstraintVectorX");
      creatables.push_back("BodyConstraintVectorY");
      creatables.push_back("BodyConstraintVectorZ");

      creatables.push_back("Attitude");
      creatables.push_back("AttitudeReferenceBody");
      creatables.push_back("AttitudeConstraintType");

      // Ballistic/Mass parameters
      creatables.push_back("DryMass");
      creatables.push_back("DryCenterOfMassX");
      creatables.push_back("DryCenterOfMassY");
      creatables.push_back("DryCenterOfMassZ");
      creatables.push_back("DryMomentOfInertiaXX");
      creatables.push_back("DryMomentOfInertiaXY");
      creatables.push_back("DryMomentOfInertiaXZ");
      creatables.push_back("DryMomentOfInertiaYY");
      creatables.push_back("DryMomentOfInertiaYZ");
      creatables.push_back("DryMomentOfInertiaZZ");
      creatables.push_back("Cd");
      creatables.push_back("Cr");
      creatables.push_back("DragArea");
      creatables.push_back("SRPArea");
      creatables.push_back("TotalMass");
      creatables.push_back("SystemCenterOfMassX");
      creatables.push_back("SystemCenterOfMassY");
      creatables.push_back("SystemCenterOfMassZ");
      creatables.push_back("SystemMomentOfInertiaXX");
      creatables.push_back("SystemMomentOfInertiaXY");
      creatables.push_back("SystemMomentOfInertiaXZ");
      creatables.push_back("SystemMomentOfInertiaYY");
      creatables.push_back("SystemMomentOfInertiaYZ");
      creatables.push_back("SystemMomentOfInertiaZZ");
      creatables.push_back("SPADDragScaleFactor");
      creatables.push_back("SPADSRPScaleFactor");
      creatables.push_back("AtmosDensityScaleFactor");
      creatables.push_back("CdSigma");
      creatables.push_back("CrSigma");
      creatables.push_back("SPADDragScaleFactorSigma");
      creatables.push_back("SPADSRPScaleFactorSigma");
      creatables.push_back("AtmosDensityScaleFactorSigma");
      
      // Orbit STM parameters
      creatables.push_back("OrbitSTM");
      creatables.push_back("OrbitSTMA");
      creatables.push_back("OrbitSTMB");
      creatables.push_back("OrbitSTMC");
      creatables.push_back("OrbitSTMD");

      // Orbit covariance parameters
      creatables.push_back("OrbitErrorCovariance");

      // FuelTank parameters
      creatables.push_back("FuelMass");
      creatables.push_back("FuelCenterOfMassX");
      creatables.push_back("FuelCenterOfMassY");
      creatables.push_back("FuelCenterOfMassZ");
      creatables.push_back("FuelMomentOfInertiaXX");
      creatables.push_back("FuelMomentOfInertiaXY");
      creatables.push_back("FuelMomentOfInertiaXZ");
      creatables.push_back("FuelMomentOfInertiaYY");
      creatables.push_back("FuelMomentOfInertiaYZ");
      creatables.push_back("FuelMomentOfInertiaZZ");
      creatables.push_back("FuelCenterOfMassX_BCS");
      creatables.push_back("FuelCenterOfMassY_BCS");
      creatables.push_back("FuelCenterOfMassZ_BCS");
      creatables.push_back("FuelMomentOfInertiaXX_BCS");
      creatables.push_back("FuelMomentOfInertiaXY_BCS");
      creatables.push_back("FuelMomentOfInertiaXZ_BCS");
      creatables.push_back("FuelMomentOfInertiaYY_BCS");
      creatables.push_back("FuelMomentOfInertiaYZ_BCS");
      creatables.push_back("FuelMomentOfInertiaZZ_BCS");
      creatables.push_back("Pressure");
      creatables.push_back("Temperature");
      creatables.push_back("RefTemperature");
      creatables.push_back("Volume");
      creatables.push_back("FuelDensity");
      
      // Hardware parameters
      creatables.push_back("R_SB11");
      creatables.push_back("R_SB12");
      creatables.push_back("R_SB13");
      creatables.push_back("R_SB21");
      creatables.push_back("R_SB22");
      creatables.push_back("R_SB23");
      creatables.push_back("R_SB31");
      creatables.push_back("R_SB32");
      creatables.push_back("R_SB33");

      // Thruster parameters
      creatables.push_back("DutyCycle");
      creatables.push_back("ThrustScaleFactor");
      creatables.push_back("GravitationalAccel");
      creatables.push_back("ThrustMagnitude");
      creatables.push_back("Isp");
      creatables.push_back("MassFlowRate");
      
      creatables.push_back("C1");
      creatables.push_back("C2");
      creatables.push_back("C3");
      creatables.push_back("C4");
      creatables.push_back("C5");
      creatables.push_back("C6");
      creatables.push_back("C7");
      creatables.push_back("C8");
      creatables.push_back("C9");
      creatables.push_back("C10");
      creatables.push_back("C11");
      creatables.push_back("C12");
      creatables.push_back("C13");
      creatables.push_back("C14");
      creatables.push_back("C15");
      creatables.push_back("C16");
      
      creatables.push_back("K1");
      creatables.push_back("K2");
      creatables.push_back("K3");
      creatables.push_back("K4");
      creatables.push_back("K5");
      creatables.push_back("K6");
      creatables.push_back("K7");
      creatables.push_back("K8");
      creatables.push_back("K9");
      creatables.push_back("K10");
      creatables.push_back("K11");
      creatables.push_back("K12");
      creatables.push_back("K13");
      creatables.push_back("K14");
      creatables.push_back("K15");
      creatables.push_back("K16");
      
      creatables.push_back("ThrustDirection1");
      creatables.push_back("ThrustDirection2");
      creatables.push_back("ThrustDirection3");

      creatables.push_back("TotalPowerAvailable");
      creatables.push_back("RequiredBusPower");
      creatables.push_back("ThrustPowerAvailable");

      creatables.push_back("OrbitTime");

      //Plate createables
      creatables.push_back("AreaCoefficient");
      creatables.push_back("SpecularFraction");
      creatables.push_back("DiffuseFraction");
      //creatables.push_back("Type");
      creatables.push_back("Area");
      creatables.push_back("LitFraction");
      creatables.push_back("PlateNormal");
      creatables.push_back("PlateNormalHistoryFile");
      creatables.push_back("AreaCoefficientSigma");
      creatables.push_back("SpecularFractionSigma");
      creatables.push_back("DiffuseFractionSigma");
      creatables.push_back("PlateX");
      creatables.push_back("PlateY");
      creatables.push_back("PlateZ");
   }

   GmatType::RegisterType(Gmat::PARAMETER, "Parameter");
   GmatType::RegisterType(Gmat::VARIABLE, "Variable");
   GmatType::RegisterType(Gmat::ARRAY, "Array");
   GmatType::RegisterType(Gmat::STRING, "String");
}


//------------------------------------------------------------------------------
//  ParameterFactory(StringArray createList)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ParameterFactory
 * (constructor).
 *
 * @param <createList> initial list of creatable objects
 *
 */
//------------------------------------------------------------------------------
ParameterFactory::ParameterFactory(StringArray createList) :
   Factory(createList, Gmat::PARAMETER)
{
}


//------------------------------------------------------------------------------
//  ParameterFactory(const ParameterFactory &fact)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the class ParameterFactory
 * (copy constructor).
 *
 * @param <fact> the factory object to copy to "this" factory.
 */
//------------------------------------------------------------------------------
ParameterFactory::ParameterFactory(const ParameterFactory &fact) :
   Factory(fact)
{
}


//------------------------------------------------------------------------------
//  ParameterFactory& operator= (const ParameterFactory &fact)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the ParameterFactory base class.
 *
 * @param <fact> the ParameterFactory object whose data to assign to "this"
 *  factory.
 *
 * @return "this" ParameterFactory with data of input factory fact.
 */
//------------------------------------------------------------------------------
ParameterFactory& ParameterFactory::operator= (const ParameterFactory &fact)
{
   Factory::operator=(fact);
   return *this;
}


//------------------------------------------------------------------------------
// ~ParameterFactory()
//------------------------------------------------------------------------------
/**
 * Destructor for the ParameterFactory base class.
 */
//------------------------------------------------------------------------------
ParameterFactory::~ParameterFactory()
{
   // deletes handled by Factory destructor
}


