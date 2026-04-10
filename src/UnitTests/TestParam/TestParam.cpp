//------------------------------------------------------------------------------
//                                  TestParam
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
// Author: Linda Jun
// Created: 2003/09/18
//
// Modifications:
//    2005/01/21 Linda Jun - Added CoordinateSystem to orbit related parameters.
//
/**
 * Test driver for parameters
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"

#include "Variable.hpp"
#include "Array.hpp"
#include "Rvector6.hpp"
#include "StringVar.hpp"

#include "TimeParameters.hpp"
#include "CartesianParameters.hpp"
#include "KeplerianParameters.hpp"
#include "SphericalParameters.hpp"
#include "AngularParameters.hpp"
#include "OrbitalParameters.hpp"
#include "EnvParameters.hpp"
#include "PlanetParameters.hpp"

#include "Spacecraft.hpp"
#include "SolarSystem.hpp"

// coordinate system
#include "CoordinateSystem.hpp"
#include "MJ2000EqAxes.hpp"
#include "MJ2000EcAxes.hpp"
#include "BodyFixedAxes.hpp"

// files
#include "TimeSystemConverter.hpp"
#include "LeapSecsFileReader.hpp"
#include "EopFile.hpp"
#include "SlpFile.hpp"

#include "ParameterException.hpp"
#include "StringTokenizer.hpp"
#include "TestOutput.hpp"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{

   const std::string *descs;
   std::string *vals;
   Integer size;
   bool boolVal;
   Real realVal;
   Real expResult;
   Rvector6 r6Val;

   // files needed for testing
   std::string eopFileName    = "C:\\projects\\gmat\\files\\planetary_coeff\\eopc04.62-now";
   std::string nutFileName    = "C:\\projects\\gmat\\files\\planetary_coeff\\NUTATION.DAT";
   std::string planFileName   = "C:\\projects\\gmat\\files\\planetary_coeff\\NUT85.DAT";
   std::string LeapFileName   = "C:\\projects\\gmat\\files\\time\\tai-utc.dat";
   
   // for time
   LeapSecsFileReader* lsFile = new LeapSecsFileReader(LeapFileName);
   lsFile->Initialize();
   EopFile *eopFile = new EopFile(eopFileName);
   eopFile->Initialize();
   ItrfCoefficientsFile* itrfFile = new ItrfCoefficientsFile(nutFileName, planFileName);
   itrfFile->Initialize();
   TimeConverterUtil::SetLeapSecsFileReader(lsFile);
   TimeConverterUtil::SetEopFile(eopFile);

   // for SolarSystem, internal CoordinateSystem
   SolarSystem *ssPtr = new SolarSystem("MySolarSystem");

   // set J2000Body for Earth
   CelestialBody *earthPtr = ssPtr->GetBody("Earth");
   std::string j2000BodyName = "Earth";
   CelestialBody *j2000Body = earthPtr;
   earthPtr->SetJ2000BodyName(j2000BodyName);
   earthPtr->SetJ2000Body(j2000Body);

   // set J2000Body for Sun
   CelestialBody *sunPtr = ssPtr->GetBody("Sun");
   sunPtr->SetJ2000BodyName("Earth");
   sunPtr->SetJ2000Body(j2000Body);

   CoordinateSystem *csPtr = new CoordinateSystem("CoordinateSystem", "EarthMJ2000Eq");
   AxisSystem *mj2000EqAxis = new MJ2000EqAxes("MJ2000Eq");
   csPtr->SetRefObject(mj2000EqAxis, Gmat::AXIS_SYSTEM, mj2000EqAxis->GetName());
   csPtr->SetStringParameter("Origin", "Earth");
   csPtr->SetStringParameter("J2000Body", "Earth");
   csPtr->SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");

   // set SLP file to SolarSystem
   std::string slpFileName = "C:\\projects\\gmat\\files\\planetary_ephem\\slp\\mn2000.pc";
   SlpFile *theSlpFile = new SlpFile(slpFileName);
   ssPtr->SetSource(Gmat::SLP);
   ssPtr->SetSourceFile(theSlpFile);

   // for Spacecraft
   Spacecraft *scPtr = new Spacecraft("MySpacecraft");
   scPtr->SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM);
      
   out.Put("***************************** test Parameter");
   out.Put("NOTE: All results are dependent on Spacecraft defaults\n");
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test TimeParameters\n");
   //---------------------------------------------------------------------------

   out.Put("==================== test CurrA1MJD()");
   CurrA1MJD ctParam = CurrA1MJD();
   
   out.Put("----- test CurrA1MJD('')");
   CurrA1MJD ctParam1 = CurrA1MJD("");
   
   out.Put("----- test AddRefObject(scPtr)");
   ctParam1.AddRefObject(scPtr);
   out.Put("num objects = ", ctParam1.GetNumRefObjects());
   
   out.Put("----- test ctParam1.EvaluateReal()");
   realVal = ctParam1.EvaluateReal();  
   out.Validate(realVal, 21545.0);
   
   out.Put("----- test CurrA1MJD('CurrA1MJD', scPtr)");
   CurrA1MJD ctParam2 = CurrA1MJD("CurrA1MJD", scPtr);
   
   out.Put("----- test CurrA1MJD(Current Time) -> should replace blank with _");
   CurrA1MJD etParamBlank = CurrA1MJD("Current Time");
   out.Put(etParamBlank.GetName());
   
   out.Put("----- test ctParam2.EvaluateReal()");
   
   realVal = ctParam2.EvaluateReal();
   out.Validate(realVal, 21545.0);
   
   out.Put("----- test GmatBase:GetName()");
   out.Put(ctParam2.GetName());
   
   out.Put("----- test GmatBase:GetType()");
   out.Put(ctParam2.GetTypeName());
   
   out.Put("----- test CurrA1MJD ctParam3(ctParam2)");
   
   CurrA1MJD ctParam3(ctParam2);
   out.Put("name = ", ctParam3.GetName());
   
   out.Put("type name = ", ctParam3.GetTypeName());
   
   out.Put("----- GetParameterCount() -> should return 6");
   out.Put(ctParam3.GetParameterCount());
   
   out.Put("----- test setting wrong object in the constructor");
   CurrA1MJD wrongParam1 = CurrA1MJD("WrongParam1", &ctParam3);
   out.Put("num objects = ", wrongParam1.GetNumRefObjects());
   
   out.Put("----- test setting wrong object in the AddRefObject");
   
   CurrA1MJD wrongParam2 = CurrA1MJD("WrongParam2");
   wrongParam2.AddRefObject(&ctParam3);
   out.Put("num objects = ", wrongParam2.GetNumRefObjects());
   
   out.Put("----- Validate() -> should return 0");
   
   boolVal = wrongParam2.Validate();
   out.Put("Validate() = ", boolVal);
   out.Put("");
   
   out.Put("==================== test ElapsedDays('ElapsedDays', scPtr)");
   
   ElapsedDays edParam1 = ElapsedDays("ElapsedDays", scPtr);
   
   out.Put("----- GetParameterID('InitialEpoch')");
   
   Integer ieId = edParam1.GetParameterID("InitialEpoch");
   out.Put(ieId);
   
   out.Put("----- SetRealParameter('ieId', 21500)");
   edParam1.SetRealParameter(ieId, 21500);
   out.Put(ieId);
   
   out.Put("----- EvaluateReal() -> should return 45.0");
   realVal = edParam1.EvaluateReal();
   out.Validate(realVal, 45.0);
   
   out.Put("----- GetParameterCount() -> should return 7");
   out.Put(edParam1.GetParameterCount());

   out.Put("----- GetRealParameter(paramId) -> should return 45.0");
   realVal = edParam1.GetReal();
   out.Validate(realVal, 45.0);
   
   out.Put("----- GetRealParameter(ieId) -> should return 21500.0");
   realVal = edParam1.GetRealParameter(ieId);
   out.Validate(realVal, 21500.0);
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test CartesianParamters\n");
   //---------------------------------------------------------------------------
   out.Put("==================== test CartX('CartX', scPtr)");
   
   CartX cartXParam = CartX("CartX", scPtr);
   cartXParam.SetSolarSystem(ssPtr);
   cartXParam.SetInternalCoordSystem(csPtr);
   cartXParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartXParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartXParam.Initialize();
   
   out.Put("----- EvaluateReal() Should return 7100.0");
   realVal = cartXParam.EvaluateReal();
   out.Validate(realVal, 7100.0);
   
   out.Put("----- GetReal() Should return 7100.0");
   realVal = cartXParam.GetReal();
   out.Validate(realVal, 7100.0);
   
   out.Put("==================== test CartY('CartY', scPtr)");
   CartY cartYParam = CartY("CartY", scPtr);
   cartYParam.SetSolarSystem(ssPtr);
   cartYParam.SetInternalCoordSystem(csPtr);
   cartYParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartYParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartYParam.Initialize();
   
   out.Put("----- Should return 0.0");
   realVal = cartYParam.EvaluateReal();
   out.Put(realVal); out.Put("");
   
   out.Put("==================== test CartZ('CartZ', scPtr)");
   CartZ cartZParam = CartZ("CartZ", scPtr);
   cartZParam.SetSolarSystem(ssPtr);
   cartZParam.SetInternalCoordSystem(csPtr);
   cartZParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartZParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartZParam.Initialize();
   
   out.Put("----- Should return 1300.0");
   realVal = cartZParam.EvaluateReal();
   out.Put(realVal); out.Put("");
   
   out.Put("==================== test CartVx('CartVx', scPtr)");
   CartVx cartVxParam = CartVx("CartVx", scPtr);
   cartVxParam.SetSolarSystem(ssPtr);
   cartVxParam.SetInternalCoordSystem(csPtr);
   cartVxParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartVxParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartVxParam.Initialize();
   
   out.Put("----- Should return 0.0");
   realVal = cartVxParam.EvaluateReal();
   out.Put(realVal); out.Put("");
   
   out.Put("==================== test CartVy('CartVy', scPtr)");
   CartVy cartVyParam = CartVy("CartVy", scPtr);
   cartVyParam.SetSolarSystem(ssPtr);
   cartVyParam.SetInternalCoordSystem(csPtr);
   cartVyParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartVyParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartVyParam.Initialize();
   
   out.Put("----- Should return 7.35");
   realVal = cartVyParam.EvaluateReal();
   out.Put(realVal); out.Put("");
   
   out.Put("==================== test CartVz('CartVz', scPtr)");
   CartVz cartVzParam = CartVz("CartVz", scPtr);
   cartVzParam.SetSolarSystem(ssPtr);
   cartVzParam.SetInternalCoordSystem(csPtr);
   cartVzParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartVzParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartVzParam.Initialize();
   
   out.Put("----- Should return 1.0");
   realVal = cartVzParam.EvaluateReal();
   out.Put(realVal); out.Put("");
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test KeplerianParamters\n");
   //---------------------------------------------------------------------------
   
   out.Put("==================== test KepSMA('KepSMA', scPtr)");
   KepSMA kepSmaParam = KepSMA("KepSMA", scPtr);
   kepSmaParam.SetSolarSystem(ssPtr);
   kepSmaParam.SetInternalCoordSystem(csPtr);
   kepSmaParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepSmaParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepSmaParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   kepSmaParam.Initialize();
   
   out.Put("----- EvaluateReal() Should return 7191.93882");
   realVal = kepSmaParam.EvaluateReal();
   out.Validate(realVal, 7191.93882);
   
   out.Put("----- GetReal() Should return 7191.93882");
   realVal = kepSmaParam.GetReal();
   out.Validate(realVal, 7191.93882);
   
   out.Put("==================== test KepEcc('KepEcc', scPtr)");
   KepEcc kepEccParam = KepEcc("KepEcc", scPtr);
   kepEccParam.SetSolarSystem(ssPtr);
   kepEccParam.SetInternalCoordSystem(csPtr);
   kepEccParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepEccParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepEccParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   kepEccParam.Initialize();
   
   out.Put("----- Should return 0.02455");
   realVal = kepEccParam.EvaluateReal();
   out.Validate(realVal, 0.02455);
   
   out.Put("==================== test KepInc('KepInc', scPtr)");
   KepInc kepIncParam = KepInc("KepInc", scPtr);
   kepIncParam.SetSolarSystem(ssPtr);
   kepIncParam.SetInternalCoordSystem(csPtr);
   kepIncParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepIncParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepIncParam.Initialize();
   
   out.Put("----- Should return 0.22427 (rad)");
   out.Put("----- 12.850080056580971 (deg)");
   realVal = kepIncParam.EvaluateReal();
   out.Validate(realVal, 12.8500800);
   
   out.Put("==================== test KepRAAN('KepRAAN', scPtr)");
   KepRAAN kepRaanParam = KepRAAN("KepRAAN", scPtr);
   kepRaanParam.SetSolarSystem(ssPtr);
   kepRaanParam.SetInternalCoordSystem(csPtr);
   kepRaanParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepRaanParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepRaanParam.Initialize();
   
   out.Put("----- Should return 5.35135 (rad)");
   out.Put("----- 306.61480219479836 (deg)");
   realVal = kepRaanParam.EvaluateReal();
   out.Validate(realVal, 306.61480219479836);
   
   out.Put("==================== test KepRADN('KepRADN', scPtr)");
   KepRADN kepRadnParam = KepRADN("KepRADN", scPtr);
   kepRadnParam.SetSolarSystem(ssPtr);
   kepRadnParam.SetInternalCoordSystem(csPtr);
   kepRadnParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepRadnParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepRadnParam.Initialize();
   
   out.Put("----- Should return 5.35135 + 2PI (rad)");
   out.Put("----- 306.61480219479836 + 180(deg)");
   realVal = kepRadnParam.EvaluateReal();
   out.Validate(realVal, 126.61480219479836);
   
   out.Put("==================== test KepAOP('KepAOP', scPtr)");
   KepAOP kepAopParam = KepAOP("KepAOP", scPtr);
   kepAopParam.SetSolarSystem(ssPtr);
   kepAopParam.SetInternalCoordSystem(csPtr);
   kepAopParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepAopParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepAopParam.Initialize();
   
   out.Put("----- Should return 5.48365 (rad)");
   out.Put("----- 314.19051361920828 (deg)");
   expResult = 314.19051361;
   realVal = kepAopParam.EvaluateReal();
   out.Validate(realVal, expResult, 0.0001); //loj: 7/13/05 changed tol from 0.01
   
   out.Put("==================== test KepMA('KepMA', scPtr)");
   KepMA kepMaParam = KepMA("KepMA", scPtr);
   kepMaParam.SetSolarSystem(ssPtr);
   kepMaParam.SetInternalCoordSystem(csPtr);
   kepMaParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepMaParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepMaParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   kepMaParam.Initialize();
   
   out.Put("----- Should return 1.791468 (rad)");
   out.Put("----- 97.107864568218005 (deg)");
   expResult = 97.107864;
   realVal = kepMaParam.EvaluateReal();
   out.Validate(realVal, expResult, 0.0001);
   
   out.Put("==================== test KepTA('KepTA', scPtr)");
   KepTA kepTaParam = KepTA("KepTA", scPtr);
   kepTaParam.SetSolarSystem(ssPtr);
   kepTaParam.SetInternalCoordSystem(csPtr);
   kepTaParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepTaParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepTaParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   kepTaParam.Initialize();
   
   out.Put("----- Should return 1.74337 (rad)");
   out.Put("----- 99.887787248832382 (deg)");
   expResult = 99.887787248;
   realVal = kepTaParam.EvaluateReal();
   out.Validate(realVal, expResult, 0.0001);
   
   out.Put("==================== test KepMM('KepMM', scPtr)");
   KepMM kepMmParam = KepMM("KepMM", scPtr);
   kepMmParam.SetSolarSystem(ssPtr);
   kepMmParam.SetInternalCoordSystem(csPtr);
   kepMmParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepMmParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepMmParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   kepMmParam.Initialize();
   
   out.Put("----- Should return 0.001035 (rad/sec)\n Swingby returns 3.558556 (deg/min)\n"
       "      assuming Earth Grav = 0.398600448073446198e+06 ");
   
   realVal = kepMmParam.EvaluateReal();
   out.Validate(realVal, 0.001035);
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test SphericalParamters\n");
   //---------------------------------------------------------------------------
   
   out.Put("==================== test RMAG('RMAG', scPtr)");
   SphRMag sphRmagParam = SphRMag("RMAG", scPtr);
   sphRmagParam.SetSolarSystem(ssPtr);
   sphRmagParam.SetInternalCoordSystem(csPtr);
   sphRmagParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphRmagParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphRmagParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   sphRmagParam.Initialize();
   
   expResult = 7218.0329730474296;
   out.Put("----- EvaluateReal() Should return ", expResult);
   realVal = sphRmagParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   out.Put("----- GetReal() Should return ", expResult);
   realVal = sphRmagParam.GetReal();
   out.Validate(realVal, expResult);
   
   out.Put("==================== test SphRA('SphRA', scPtr)");
   SphRA sphRaParam = SphRA("SphRA", scPtr);
   sphRaParam.SetSolarSystem(ssPtr);
   sphRaParam.SetInternalCoordSystem(csPtr);
   sphRaParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphRaParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphRaParam.Initialize();
   
   out.Put("----- Should return 0.0");
   out.Put("Swingby returns 360.0");
   realVal = sphRaParam.EvaluateReal();
   out.Put(realVal); out.Put("");

   out.Put("==================== test SphDec('SphDec', scPtr)");
   SphDec sphDecParam = SphDec("SphDec", scPtr);
   sphDecParam.SetSolarSystem(ssPtr);
   sphDecParam.SetInternalCoordSystem(csPtr);
   sphDecParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphDecParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphDecParam.Initialize();
   
   out.Put("----- Should return 10.375844920050977");
   realVal = sphDecParam.EvaluateReal();
   out.Put(realVal); out.Put("");

   out.Put("==================== test SphRAV('SphRAV', scPtr)");
   SphRAV sphRavParam = SphRAV("SphRAV", scPtr);
   sphRavParam.SetSolarSystem(ssPtr);
   sphRavParam.SetInternalCoordSystem(csPtr);
   sphRavParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphRavParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphRavParam.Initialize();
   
   out.Put("----- Should return 89.999999999999687");
   expResult = 89.999999999999687;
   realVal = sphRavParam.EvaluateReal();
   out.Validate(realVal, expResult);

   out.Put("==================== test SphDecV('SphDecV', scPtr)");
   SphDecV sphDecvParam = SphDecV("SphDecV", scPtr);
   sphDecvParam.SetSolarSystem(ssPtr);
   sphDecvParam.SetInternalCoordSystem(csPtr);
   sphDecvParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphDecvParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphDecvParam.Initialize();
   
   out.Put("----- Should return 7.7477720361081701");
   expResult = 7.7477720361081701;
   realVal = sphDecvParam.EvaluateReal();
   out.Validate(realVal, expResult);

   out.Put("==================== test SphAzi('SphAzi', scPtr)");
   SphAzi sphAziParam = SphAzi("SphAzi", scPtr);
   sphAziParam.SetSolarSystem(ssPtr);
   sphAziParam.SetInternalCoordSystem(csPtr);
   sphAziParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphAziParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphAziParam.Initialize();
   
   out.Put("----- Should return 82.377421681550345");
   expResult = 82.377421681550345;
   realVal = sphAziParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   out.Put("==================== test SphFPA('SphFPA', scPtr)");
   SphFPA sphFpaParam = SphFPA("SphFPA", scPtr);
   sphFpaParam.SetSolarSystem(ssPtr);
   sphFpaParam.SetInternalCoordSystem(csPtr);
   sphFpaParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphFpaParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphFpaParam.Initialize();
   
   out.Put("----- Should return 88.608703653704666");
   expResult = 88.608703653704666;
   realVal = sphFpaParam.EvaluateReal();
   out.Validate(realVal, expResult);

   out.Put("==================== test Altitude('Altitude', scPtr)");
   Altitude altitudeParam = Altitude("Altitude", scPtr);
   altitudeParam.SetSolarSystem(ssPtr);
   altitudeParam.SetInternalCoordSystem(csPtr);
   altitudeParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   altitudeParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   altitudeParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   altitudeParam.Initialize();
   
   expResult = 839.896673;
   out.Put("Should return ", expResult);
   realVal = altitudeParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test AngularParameters\n");
   //---------------------------------------------------------------------------
   
   out.Put("==================== test SemilatusRectum('SemilatusRectum', scPtr)");
   SemilatusRectum semilatusRectumParam = SemilatusRectum("SemilatusRectum", scPtr);
   semilatusRectumParam.SetSolarSystem(ssPtr);
   semilatusRectumParam.SetInternalCoordSystem(csPtr);
   semilatusRectumParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   semilatusRectumParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   semilatusRectumParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   semilatusRectumParam.Initialize();
   
   out.Put("EvaluateReal() Should return 7187.604188 (km)");
   expResult = 7187.604188;
   realVal = semilatusRectumParam.EvaluateReal();
   out.Validate(realVal, expResult, 0.001);
   
   out.Put("GetReal() Should return 7187.604188 (km)");
   realVal = semilatusRectumParam.GetReal();
   out.Validate(realVal, expResult, 0.001);
   
   out.Put("==================== test AngularMomentumMag('AngularMomentumMag', scPtr)");
   AngularMomentumMag angularMomentumMagParam = AngularMomentumMag("AngularMomentumMag", scPtr);
   angularMomentumMagParam.SetSolarSystem(ssPtr);
   angularMomentumMagParam.SetInternalCoordSystem(csPtr);
   angularMomentumMagParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   angularMomentumMagParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   angularMomentumMagParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   angularMomentumMagParam.Initialize();
   
   out.Put("----- Should return 53525.528955816953 (km*km/s)");
   expResult = 53525.5289558;
   realVal = angularMomentumMagParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   out.Put("==================== test AngularMomentumX('AngularMomentumX', scPtr)");
   AngularMomentumX angularMomentumXParam = AngularMomentumX("AngularMomentumX", scPtr);
   angularMomentumXParam.SetSolarSystem(ssPtr);
   angularMomentumXParam.SetInternalCoordSystem(csPtr);
   angularMomentumXParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   angularMomentumXParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   angularMomentumXParam.Initialize();
   
   out.Put("----- Should return -9555.0 (km*km/s)");
   expResult = -9555.0;
   realVal = angularMomentumXParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   out.Put("==================== test AngularMomentumY('AngularMomentumY', scPtr)");
   AngularMomentumY angularMomentumYParam = AngularMomentumY("AngularMomentumY", scPtr);
   angularMomentumYParam.SetSolarSystem(ssPtr);
   angularMomentumYParam.SetInternalCoordSystem(csPtr);
   angularMomentumYParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   angularMomentumYParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   angularMomentumYParam.Initialize();
   
   out.Put("Should return -7100 (km*km/s)");
   expResult = -7100.0;
   realVal = angularMomentumYParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   out.Put("==================== test AngularMomentumZ('AngularMomentumZ', scPtr)");
   AngularMomentumZ angularMomentumZParam = AngularMomentumZ("AngularMomentumZ", scPtr);
   angularMomentumZParam.SetSolarSystem(ssPtr);
   angularMomentumZParam.SetInternalCoordSystem(csPtr);
   angularMomentumZParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   angularMomentumZParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   angularMomentumZParam.Initialize();
   
   out.Put("Should return 52185 (km*km/s)");
   expResult = 52185.0;
   realVal = angularMomentumZParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   out.Put("==================== test new BetaAngle('betaAngle')");
   BetaAngle *betaAngle = new BetaAngle("betaAngle");
   betaAngle->SetRefObjectName(Gmat::SOLAR_SYSTEM, "MySolarSystem");
   betaAngle->SetRefObjectName(Gmat::SPACECRAFT, "MySpacecraft");
   betaAngle->SetRefObject(ssPtr, Gmat::SOLAR_SYSTEM, "MySolarSystem");
   betaAngle->SetRefObject(scPtr, Gmat::SPACECRAFT, "MySpacecraft");
   betaAngle->SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   betaAngle->SetInternalCoordSystem(csPtr);
   betaAngle->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   betaAngle->SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   betaAngle->Initialize();
   
   expResult = 107.09245;
   out.Put("----- Should return ", expResult, "Swingby returns ???");
   realVal = betaAngle->EvaluateReal();
   out.Validate(realVal, expResult);
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test OrbitalParameters\n");
   //---------------------------------------------------------------------------
   
   out.Put("==================== test VelApoapsis('VelApoapsis', scPtr)");
   VelApoapsis velApoapsisParam = VelApoapsis("VelApoapsis", scPtr);
   velApoapsisParam.SetSolarSystem(ssPtr);
   velApoapsisParam.SetInternalCoordSystem(csPtr);
   velApoapsisParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   velApoapsisParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   velApoapsisParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   velApoapsisParam.Initialize();
   
   out.Put("----- EvaluateReal() Should return 7.264102 (km/s)");
   out.Put("      assuming Earth Grav = 3.986005e6 ");
   expResult = 7.264102;
   realVal = velApoapsisParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   out.Put("----- GetReal() Should return 7.264102 (km/s)");
   realVal = velApoapsisParam.GetReal();
   out.Validate(realVal, expResult);
   
   out.Put("==================== test VelPeriapsis('VelPeriapsis', scPtr)");
   VelPeriapsis velPeriapsisParam = VelPeriapsis("VelPeriapsis", scPtr);
   velPeriapsisParam.SetSolarSystem(ssPtr);
   velPeriapsisParam.SetInternalCoordSystem(csPtr);
   velPeriapsisParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   velPeriapsisParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   velPeriapsisParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   velPeriapsisParam.Initialize();
   
   out.Put("----- Should return 7.62974 (km/s)\n      assuming Earth Grav = 3.986005e6");
   expResult = 7.62974;
   realVal = velPeriapsisParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   out.Put("==================== test OrbitPeriod('OrbitPeriod', scPtr)");
   OrbitPeriod orbitPeriodParam = OrbitPeriod("OrbitPeriod", scPtr);
   orbitPeriodParam.SetSolarSystem(ssPtr);
   orbitPeriodParam.SetInternalCoordSystem(csPtr);
   orbitPeriodParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   orbitPeriodParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   orbitPeriodParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   orbitPeriodParam.Initialize();
   
   expResult = 101.16462878847361 * 60.0; //101.164628788 min * 60
   out.Put("----- Should return ", expResult, "\n      assuming Earth Grav = 3.986005e6");
   realVal = orbitPeriodParam.EvaluateReal();
   out.Validate(realVal, expResult, 0.001);

   out.Put("==================== test RadPeriapsis('RadPer', scPtr)");
   RadPeriapsis radPerParam = RadPeriapsis("RadPer", scPtr);
   radPerParam.SetSolarSystem(ssPtr);
   radPerParam.SetInternalCoordSystem(csPtr);
   radPerParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   radPerParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   radPerParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   radPerParam.Initialize();
   
   out.Put("----- Should return 7015.3783924785976 km\n      assuming Earth Grav = 3.986005e6");
   expResult = 7015.37839247;
   realVal = radPerParam.EvaluateReal();
   out.Validate(realVal, expResult, 0.001);
   
   out.Put("==================== test RadApoapsis('RadApo', scPtr)");
   RadApoapsis radApoParam = RadApoapsis("RadApo", scPtr);
   radApoParam.SetSolarSystem(ssPtr);
   radApoParam.SetInternalCoordSystem(csPtr);
   radApoParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   radApoParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   radApoParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   radApoParam.Initialize();
   
   out.Put("----- Should return 7368.4990072854334 km\n      assuming Earth Grav = 3.986005e6");
   expResult = 7368.4990072854334;
   realVal = radApoParam.EvaluateReal();
   out.Validate(realVal, expResult, 0.001);
   
   out.Put("==================== test C3Energy('C3Energy', scPtr)");
   C3Energy c3EnergyParam = C3Energy("C3Energy", scPtr);
   c3EnergyParam.SetSolarSystem(ssPtr);
   c3EnergyParam.SetInternalCoordSystem(csPtr);
   c3EnergyParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   c3EnergyParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   c3EnergyParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   c3EnergyParam.Initialize();
   
   out.Put("----- Should return -55.423226574495871 km*km/s*s\n"
           "      assuming Earth Grav = 3.986005e6");
   expResult = -55.42322657449;
   realVal = c3EnergyParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   out.Put("==================== test Energy('Energy', scPtr)");
   Energy energyParam = Energy("Energy", scPtr);
   energyParam.SetSolarSystem(ssPtr);
   energyParam.SetInternalCoordSystem(csPtr);
   energyParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   energyParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   energyParam.SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   energyParam.Initialize();
   
   out.Put("----- Should return  -27.711613287247935 km*km/s*s\n"
           "      assuming Earth Grav = 3.986005e6");
   expResult = -27.711613287;
   realVal = energyParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test CartState\n");
   //---------------------------------------------------------------------------
   
   out.Put("==================== test CartState('CartState', scPtr)");
   CartState cartStateParam = CartState("CartState", scPtr);
   cartStateParam.SetSolarSystem(ssPtr);
   cartStateParam.SetInternalCoordSystem(csPtr);
   cartStateParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartStateParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartStateParam.Initialize();
   
   out.Put("----- EvaluateRvector6()");
   r6Val = cartStateParam.EvaluateRvector6();
   std::string *strVal = r6Val.ToValueStrings();
   out.Put(strVal[0] + " " + strVal[1] + " " + strVal[2] + " " +
       strVal[3] + " " + strVal[4] + " " + strVal[5]); out.Put("");
      
   out.Put("----- GetRvector6()");
   r6Val = cartStateParam.GetRvector6();
   strVal = r6Val.ToValueStrings();
   out.Put(strVal[0] + " " + strVal[1] + " " + strVal[2] + " " +
       strVal[3] + " " + strVal[4] + " " + strVal[5]); out.Put("");
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test KepElem\n");
   //---------------------------------------------------------------------------
   
   out.Put("==================== test KepElem('KepElem', scPtr)");
   KepElem kepElemParam = KepElem("KepElem", scPtr);
   kepElemParam.SetSolarSystem(ssPtr);
   kepElemParam.SetInternalCoordSystem(csPtr);
   kepElemParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepElemParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   kepElemParam.Initialize();
   
   out.Put("----- EvaluateRvector6()");
   out.Put("-> shoud return 7191.938699, 0.024549751, 12.8500800, 306.614802, "
           "314.190513, 99.887787");
   r6Val = kepElemParam.EvaluateRvector6();
   strVal = r6Val.ToValueStrings();
   out.Put(strVal[0] + " " + strVal[1] + " " + strVal[2] + " " +
       strVal[3] + " " + strVal[4] + " " + strVal[5]); out.Put("");
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test ModKepElem\n");
   //---------------------------------------------------------------------------
   
   out.Put("==================== test ModKepElem('ModKepElem', scPtr)");
   ModKepElem modKepElemParam = ModKepElem("ModKepElem", scPtr);
   modKepElemParam.SetSolarSystem(ssPtr);
   modKepElemParam.SetInternalCoordSystem(csPtr);
   modKepElemParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   modKepElemParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   modKepElemParam.Initialize();
   
   out.Put("----- EvaluateRvector6()");
   out.Put("-> shoud return 7015.378392, 7368.4990072, 12.8500800, 306.614802, "
           "314.190513, 99.887787");
   r6Val = modKepElemParam.EvaluateRvector6();
   strVal = r6Val.ToValueStrings();
   out.Put(strVal[0] + " " + strVal[1] + " " + strVal[2] + " " +
       strVal[3] + " " + strVal[4] + " " + strVal[5]); out.Put("");
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test SphRaDecElem");
   //---------------------------------------------------------------------------
   
   out.Put("==================== test SphRaDecElem('SphRaDecElem', scPtr)");
   SphRaDecElem sphRaDecParam = SphRaDecElem("SphRaDecElem", scPtr);
   sphRaDecParam.SetSolarSystem(ssPtr);
   sphRaDecParam.SetInternalCoordSystem(csPtr);
   sphRaDecParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphRaDecParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphRaDecParam.Initialize();
   
   out.Put("----- EvaluateRvector6()");
   out.Put("-> shoud return 7218.032973, 360.0, 10.37584, 7.417715, 90.0, 7.747772");
   r6Val = sphRaDecParam.EvaluateRvector6();
   strVal = r6Val.ToValueStrings();
   out.Put(strVal[0] + " " + strVal[1] + " " + strVal[2] + " " +
       strVal[3] + " " + strVal[4] + " " + strVal[5]); out.Put("");
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test SphAzFpaElem");
   //---------------------------------------------------------------------------
   
   out.Put("==================== test SphAzFpaElem('SphAzFpaElem', scPtr)");
   SphAzFpaElem sphAzFpaParam = SphAzFpaElem("SphAzFpaElem", scPtr);
   sphAzFpaParam.SetSolarSystem(ssPtr);
   sphAzFpaParam.SetInternalCoordSystem(csPtr);
   sphAzFpaParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphAzFpaParam.SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   sphAzFpaParam.Initialize();
   
   out.Put("----- EvaluateRvector6()");
   out.Put("-> shoud return 7218.032973, 360.0, 10.37584, 7.417715, 82.37742168 88.60870365");
   r6Val = sphAzFpaParam.EvaluateRvector6();
   strVal = r6Val.ToValueStrings();
   out.Put(strVal[0] + " " + strVal[1] + " " + strVal[2] + " " +
       strVal[3] + " " + strVal[4] + " " + strVal[5]); out.Put("");
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test Ref. Object\n");
   //---------------------------------------------------------------------------
   Spacecraft *sat1 = new Spacecraft("Sat1");
   CurrA1MJD *timePtr = new CurrA1MJD();
   
   out.Put("----- test SetRefObjectName()");
   timePtr->SetRefObjectName(Gmat::SPACECRAFT, "Sat1");
   
   out.Put("num objects = ", timePtr->GetNumRefObjects());
   
   out.Put("----- test GetRefObjectName()");
   std::string sat1name = timePtr->GetRefObjectName(Gmat::SPACECRAFT);
   out.Put(" sc name = ", sat1name);
   
   out.Put("----- test SetRefObject()");
   timePtr->SetRefObject(sat1, Gmat::SPACECRAFT, "Sat1");
   
   out.Put("----- test GetRefObject()");
   GmatBase *obj = timePtr->GetRefObject(Gmat::SPACECRAFT, "Sat1");
   out.Put(" sc name = ", obj->GetName()); out.Put("");
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test Variable\n");
   //---------------------------------------------------------------------------

   std::string exp;

   // test varX = 10.123
   // for varX
   Variable *varX = new Variable("varX");
   varX->SetStringParameter("Expression", "10.123");
   varX->SetRefObjectName(Gmat::PARAMETER, "varX");   // used during script parsing
   varX->SetRefObject(varX, Gmat::PARAMETER, "varX"); // used in Sandbox::Initialize()
   out.Put("----- varX exp = ", varX->GetStringParameter("Expression"));
   realVal = varX->EvaluateReal();
   out.Put("-> varX should return 10.123");
   out.Validate(realVal, 10.123);
   
   out.Put("----- now set parameter to 100.345");
   varX->SetReal(100.345);
   realVal = varX->EvaluateReal();
   out.Put("-> varX should return 100.345");
   out.Validate(realVal, 100.345);
   
   // for varA
   Variable *varA = new Variable("varA");
   varA->SetStringParameter("Expression", "123.123");
   varA->SetRefObjectName(Gmat::PARAMETER, "varA");   // used during script parsing
   varA->SetRefObject(varA, Gmat::PARAMETER, "varA"); // used in Sandbox::Initialize()
   out.Put("----- varA exp = ", varA->GetStringParameter("Expression"));
   realVal = varA->EvaluateReal();
   out.Put("-> varA should have 123.123");
   out.Validate(realVal, 123.123);
   
   // for varB
   Variable *varB = new Variable("varB");
   varB->SetStringParameter("Expression", "345.345");
   varB->SetRefObjectName(Gmat::PARAMETER, "varB");   // used during script parsing
   varB->SetRefObject(varB, Gmat::PARAMETER, "varB"); // used in Sandbox::Initialize()
   out.Put("----- varB exp = ", varB->GetStringParameter("Expression"));
   realVal = varB->EvaluateReal();
   out.Put("varB should have 345.345");
   out.Validate(realVal, 345.345);

   // for varA - use SetRealParameter() to set varB + 1000 to varA (1345.345)
   //varA->SetRealParameter("Value", varB->EvaluateReal() + 1000);
   varA->SetReal(varB->EvaluateReal() + 1000);
   realVal = varA->EvaluateReal();
   out.Put("Set varA to varB+1000, new varA = ", realVal); out.Put("");
   out.Validate(realVal, 1345.345);
   
   // for varA - use expression to set "varB + 1000" to varA
   exp = "varB + 1000";
   varA->SetStringParameter("Expression", exp);
   varA->SetRefObjectName(Gmat::PARAMETER, "varB");   // used during script parsing
   varA->SetRefObject(varB, Gmat::PARAMETER, "varB"); // used in Sandbox::Initialize()
   out.Put("----- varA exp = ", varA->GetStringParameter("Expression"));
   realVal = varA->EvaluateReal();
   out.Put("varA->EvaluateReal() = ", realVal); out.Put("");
   out.Validate(realVal, 1345.345);

   // for varC
   // use expression to set varC
   Variable *varC = new Variable("varC");
   exp = "varA * 10 + varB + 10"; // 1345.345 * 10 + 345.345 + 10 = 13808.795
   varC->SetStringParameter("Expression", exp);
   out.Put("----- varC exp = ", varC->GetStringParameter("Expression"));
   varC->SetRefObjectName(Gmat::PARAMETER, "varA");   // used during script parsing
   varC->SetRefObjectName(Gmat::PARAMETER, "varB");   // used during script parsing
   varC->SetRefObject(varA, Gmat::PARAMETER, "varA"); // used in Sandbox::Initialize()
   varC->SetRefObject(varB, Gmat::PARAMETER, "varB"); // used in Sandbox::Initialize()
   realVal = varC->EvaluateReal();   
   out.Put("varC->EvaluateReal() = ", realVal); out.Put("");
   out.Validate(realVal, 13808.795);

   // use StringTokenizer to parse ref. parameters
   Variable *varD = new Variable("varD");
   out.Put("----- use StringTokenizer to parse ref. parameters");
   exp = "varA * (varB + 10)";
   Real expVal = 1345.345 * (345.345 + 10);
   varD->SetStringParameter("Expression", exp);
   out.Put("----- varD exp = ", varD->GetStringParameter("Expression"));
   
   StringTokenizer st(exp, "()*/+-^ ");
   StringArray tokens = st.GetAllTokens();

   for (unsigned int i=0; i<tokens.size(); i++)
   {
      out.Put("token:<" + tokens[i] + ">");
      
      if (!isdigit(*tokens[i].c_str()))
         varD->SetRefObjectName(Gmat::PARAMETER, tokens[i]);
   }
   
   varD->SetRefObject(varA, Gmat::PARAMETER, "varA"); // used in Sandbox::Initialize()
   varD->SetRefObject(varB, Gmat::PARAMETER, "varB"); // used in Sandbox::Initialize()
   realVal = varD->EvaluateReal();   
   out.Put("varD->EvaluateReal() = ", realVal); out.Put("");
   out.Validate(realVal, expVal);
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test Array\n");
   //---------------------------------------------------------------------------

   Array *arrA = new Array("arrA");
   arrA->SetIntegerParameter("NumRows", 5);
   arrA->SetIntegerParameter("NumCols", 3);
   out.Put("arrA->GetMatrix() = \n", arrA->GetMatrix());

   Rvector row1Vec(3, 2.1, 2.2, 2.3);
   out.Put("row1Vec ="); out.Put(row1Vec);
   out.Put("arrA->SetRvectorParameter('RowValue', row1Vec, 1");
   arrA->SetRvectorParameter("RowValue", row1Vec, 1);
   out.Put("arrA->GetMatrix() = "); out.Put(arrA->GetMatrix());
   
   Rvector col2Vec(5, 1.3, 2.3, 3.3, 4.3, 5.3);
   out.Put("col2Vec =\n", col2Vec);
   out.Put("arrA->SetRvectorParameter('ColValue', col2Vec, 2)");
   arrA->SetRvectorParameter("ColValue", col2Vec, 2);
   out.Put("arrA->GetMatrix() ="); out.Put(arrA->GetMatrix());
   
   Rvector row2Vec(3, 1.0, 2.0, 3.0); // real number must have ., otherwise it will not work
   out.Put("row2Vec ="); out.Put(row2Vec);
   out.Put("arrA->SetRvectorParameter('RowValue', row2Vec, 2)");
   arrA->SetRvectorParameter("RowValue", row2Vec, 2);
   out.Put("arrA->GetMatrix() ="); out.Put(arrA->GetMatrix());

   out.Put("==================== test exception");
   Array *arrB = new Array("arrB");
   arrB->SetIntegerParameter("NumRows", 3);
   try
   {
      out.Put("Set NumRows again");
      arrB->SetIntegerParameter("NumRows", 3);
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   
   try
   {
      out.Put("arrB->GetMatrix() =");
      arrB->GetMatrix();
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }

   arrB->SetIntegerParameter("NumCols", 5);
   out.Put("arrB->GetMatrix() ="); out.Put(arrB->GetMatrix());
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test StringVar\n");
   //---------------------------------------------------------------------------
   StringVar *str1 = new StringVar("str1");
   str1->SetStringParameter("Expression", "My Test String");
   out.Put("str1->GetString() = ", str1->GetString());
   out.Put("str1->GetStringParameter('Expression') = ",
       str1->GetStringParameter("Expression"));
   StringVar *str2 = new StringVar("str1");
   out.Put("testing str2 = (StringVar*)str1->Clone()");
   str2 = (StringVar*)str1->Clone();
   out.Put("str2->GetStringParameter('Expression') = ",
       str2->GetStringParameter("Expression"));
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test EnvParameters\n");
   //---------------------------------------------------------------------------
   out.Put("==================== test new AtmosDensity('density1')");
   AtmosDensity *density = new AtmosDensity("density1");
   density->SetRefObjectName(Gmat::SOLAR_SYSTEM, "MySolarSystem");
   density->SetRefObjectName(Gmat::SPACECRAFT, "MySpacecraft");
   density->SetRefObject(ssPtr, Gmat::SOLAR_SYSTEM, "MySolarSystem");
   density->SetRefObject(scPtr, Gmat::SPACECRAFT, "MySpacecraft");
   density->SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   density->Initialize();
   out.Put("density = ", density->EvaluateReal());
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test PlanetParameters\n");
   //---------------------------------------------------------------------------
   out.Put("==================== test new GHA('gha')");
   GHA *gha = new GHA("gha");
   gha->SetRefObjectName(Gmat::SOLAR_SYSTEM, "MySolarSystem");
   gha->SetRefObjectName(Gmat::SPACECRAFT, "MySpacecraft");
   gha->SetRefObject(ssPtr, Gmat::SOLAR_SYSTEM, "MySolarSystem");
   gha->SetRefObject(scPtr, Gmat::SPACECRAFT, "MySpacecraft");
   gha->SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   gha->Initialize();
   out.Put("----- Should return 280.324868 Swingby returns 280.4570680");
   realVal = gha->EvaluateReal();
   out.Validate(realVal, 280.324868);
   
   out.Put("==================== test new Longitude('longitude')");
   Longitude *longitude = new Longitude("longitude");
   longitude->SetRefObjectName(Gmat::SOLAR_SYSTEM, "MySolarSystem");
   longitude->SetRefObjectName(Gmat::SPACECRAFT, "MySpacecraft");
   longitude->SetRefObject(ssPtr, Gmat::SOLAR_SYSTEM, "MySolarSystem");
   longitude->SetRefObject(scPtr, Gmat::SPACECRAFT, "MySpacecraft");
   longitude->SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   longitude->Initialize();
   out.Put("----- Should return 79.6751316 Swingby returns 79.54293193");
   realVal = longitude->EvaluateReal();
   out.Validate(realVal, 79.6751316);
   
   out.Put("==================== test new Latitude('latitude')");
   Latitude *latitude = new Latitude("latitude");
   latitude->SetRefObjectName(Gmat::SOLAR_SYSTEM, "MySolarSystem");
   latitude->SetRefObjectName(Gmat::SPACECRAFT, "MySpacecraft");
   latitude->SetRefObject(ssPtr, Gmat::SOLAR_SYSTEM, "MySolarSystem");
   latitude->SetRefObject(scPtr, Gmat::SPACECRAFT, "MySpacecraft");
   latitude->SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   latitude->Initialize();
   out.Put("----- Should return 10.3758449 Swingby returns 10.3758449");
   realVal = latitude->EvaluateReal();
   out.Validate(realVal, 10.3758449);
   
   out.Put("==================== test new LST('lst')");
   LST *lst = new LST("lst");
   lst->SetRefObjectName(Gmat::SOLAR_SYSTEM, "MySolarSystem");
   lst->SetRefObjectName(Gmat::SPACECRAFT, "MySpacecraft");
   lst->SetRefObject(ssPtr, Gmat::SOLAR_SYSTEM, "MySolarSystem");
   lst->SetRefObject(scPtr, Gmat::SPACECRAFT, "MySpacecraft");
   lst->SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   lst->Initialize();
   out.Put("----- Should return 10.6233517 Swingby returns ???");
   realVal = lst->EvaluateReal();
   out.Validate(realVal, 10.6233517);
   
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test Mars origin MarsMJ2000Eq\n");
   //---------------------------------------------------------------------------
   CoordinateSystem *marsCsPtr = new CoordinateSystem("CoordinateSystem", "MarsMJ2000Eq");
   marsCsPtr->SetRefObject(mj2000EqAxis, Gmat::AXIS_SYSTEM, mj2000EqAxis->GetName());
   marsCsPtr->SetSolarSystem(ssPtr);
   marsCsPtr->SetStringParameter("Origin", "Mars");
   marsCsPtr->SetStringParameter("J2000Body", "Earth");

   CelestialBody *marsPtr = ssPtr->GetBody("Mars");
   marsPtr->SetJ2000BodyName(j2000BodyName);
   marsPtr->SetJ2000Body(j2000Body);
   
   marsCsPtr->SetRefObject(marsPtr, Gmat::SPACE_POINT, "Mars");
   marsCsPtr->SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   marsCsPtr->Initialize();
   
   out.Put("==================== test CartX('MarsX', scPtr)");
   CartX marsXParam = CartX("MarsX", scPtr);
   marsXParam.SetSolarSystem(ssPtr);
   marsXParam.SetInternalCoordSystem(csPtr);
   marsXParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "MarsMJ2000Eq");
   marsXParam.SetRefObject(marsCsPtr, Gmat::COORDINATE_SYSTEM, "MarsMJ2000Eq");
   
   expResult = -234541070.119867;
   out.Put("----- Should return ", expResult);
   realVal = marsXParam.EvaluateReal();
   out.Validate(realVal, expResult, 0.01, false);
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test EarthMJ2000Ec\n");
   //---------------------------------------------------------------------------
   CoordinateSystem *eccsPtr = new CoordinateSystem("CoordinateSystem", "EarthMJ2000Ec");
   AxisSystem *ecAxis = new MJ2000EcAxes("MJ2000Ec");
   eccsPtr->SetRefObject(ecAxis, Gmat::AXIS_SYSTEM, ecAxis->GetName());
   eccsPtr->SetSolarSystem(ssPtr);
   eccsPtr->SetStringParameter("Origin", "Earth");
   eccsPtr->SetStringParameter("J2000Body", "Earth");
   eccsPtr->SetOrigin(earthPtr);
   eccsPtr->SetJ2000Body(earthPtr);
   eccsPtr->Initialize();
   
   out.Put("==================== test CartX('EcX', scPtr)");
   CartX ecXParam = CartX("EcX", scPtr);
   ecXParam.SetSolarSystem(ssPtr);
   ecXParam.SetInternalCoordSystem(csPtr);
   ecXParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Ec");
   ecXParam.SetRefObject(eccsPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Ec");
   
   out.Put("----- Should return 7100.0");
   expResult = 7100.0;
   realVal = ecXParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   out.Put("==================== test CartY('EcY', scPtr)");
   CartY ecYParam = CartY("EcY", scPtr);
   ecYParam.SetSolarSystem(ssPtr);
   ecYParam.SetInternalCoordSystem(csPtr);
   ecYParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Ec");
   ecYParam.SetRefObject(eccsPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Ec");
   
   expResult = 517.110302688;
   out.Put("----- Should return ", expResult);
   realVal = ecYParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   out.Put("==================== test CartZ('EcZ', scPtr)");
   CartZ ecZParam = CartZ("EcZ", scPtr);
   ecZParam.SetSolarSystem(ssPtr);
   ecZParam.SetInternalCoordSystem(csPtr);
   ecZParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Ec");
   ecZParam.SetRefObject(eccsPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Ec");
   
   expResult = 1192.72668070;
   out.Put("----- Should return ", expResult);
   realVal = ecZParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test EarthFixed\n");
   //---------------------------------------------------------------------------
   CoordinateSystem *efcsPtr = new CoordinateSystem("CoordinateSystem", "EarthFixed");
   BodyFixedAxes *bfAxis = new BodyFixedAxes("EarthFixed");

   // BodyFixedAxes needs EopFile, ItrfFile

   bfAxis->SetEopFile(eopFile);
   bfAxis->SetCoefficientsFile(itrfFile);
   
   efcsPtr->SetRefObject(bfAxis, Gmat::AXIS_SYSTEM, bfAxis->GetName());
   efcsPtr->SetSolarSystem(ssPtr);
   efcsPtr->SetStringParameter("Origin", "Earth");
   efcsPtr->SetStringParameter("J2000Body", "Earth");
   efcsPtr->SetOrigin(earthPtr);
   efcsPtr->SetJ2000Body(earthPtr);
   efcsPtr->Initialize();
   
   out.Put("==================== test CartX('EfX', scPtr)");
   CartX efXParam = CartX("EfX", scPtr);
   efXParam.SetSolarSystem(ssPtr);
   efXParam.SetInternalCoordSystem(csPtr);
   efXParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthFixed");
   efXParam.SetRefObject(efcsPtr, Gmat::COORDINATE_SYSTEM, "EarthFixed");
   
   expResult = 1272.91183866;
   out.Put("----- Should return ", expResult);
   realVal = efXParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   out.Put("==================== test CartY('EfY', scPtr)");
   CartY efYParam = CartY("EfY", scPtr);
   efYParam.SetSolarSystem(ssPtr);
   efYParam.SetInternalCoordSystem(csPtr);
   efYParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthFixed");
   efYParam.SetRefObject(efcsPtr, Gmat::COORDINATE_SYSTEM, "EarthFixed");
   
   expResult = 6984.99534448;
   out.Put("----- Should return ", expResult);
   realVal = efYParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   out.Put("==================== test CartZ('EfZ', scPtr)");
   CartZ efZParam = CartZ("EfZ", scPtr);
   efZParam.SetSolarSystem(ssPtr);
   efZParam.SetInternalCoordSystem(csPtr);
   efZParam.SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthFixed");
   efZParam.SetRefObject(efcsPtr, Gmat::COORDINATE_SYSTEM, "EarthFixed");
   
   expResult = 1299.82132946;
   out.Put("----- Should return ", expResult);
   realVal = efZParam.EvaluateReal();
   out.Validate(realVal, expResult);
   
   //---------------------------------------------
   // clean up
   //---------------------------------------------
   delete scPtr;
   delete theSlpFile;
   delete ssPtr;
   delete sat1;
   delete timePtr;
   delete varA;
   delete varB;
   delete varC;
   delete varD;
   delete arrA;
   delete arrB;
   delete str1;
   delete density;
   delete gha;
   delete longitude;
   delete latitude;
   delete lst;
   delete betaAngle;
}

//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   TestOutput out("..\\..\\Test\\TestParam\\TestParamOut.txt");
   out.SetPrecision(16);
   out.SetWidth(20);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of parameters!!");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetMessage());
   }
   catch (...)
   {
      out.Put("Unknown error occurred\n");
   }
   
   out.Close();
   
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
}
