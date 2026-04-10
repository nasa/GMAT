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
// Created: 2005/05/27
//
// Modifications:
//
/**
 * Test driver for Burn parameters.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "BurnParameters.hpp"
#include "ImpulsiveBurn.hpp"
#include "SolarSystem.hpp"

// coordinate system
#include "CoordinateSystem.hpp"
#include "MJ2000EqAxes.hpp"
// #include "MJ2000EcAxes.hpp"
// #include "BodyFixedAxes.hpp"

// files
// #include "TimeSystemConverter.hpp"
// #include "LeapSecsFileReader.hpp"
// #include "EopFile.hpp"
#include "SlpFile.hpp"

#include "TestOutput.hpp"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{

   Real realVal;
   Real expResult;
   
//    // files needed for testing
//    std::string eopFileName    = "C:\\projects\\gmat\\files\\planetary_coeff\\eopc04.62-now";
//    std::string nutFileName    = "C:\\projects\\gmat\\files\\planetary_coeff\\NUTATION.DAT";
//    std::string planFileName   = "C:\\projects\\gmat\\files\\planetary_coeff\\NUT85.DAT";
//    std::string LeapFileName   = "C:\\projects\\gmat\\files\\time\\tai-utc.dat";
   
//    // for time
//    LeapSecsFileReader* lsFile = new LeapSecsFileReader(LeapFileName);
//    lsFile->Initialize();
//    EopFile *eopFile = new EopFile(eopFileName);
//    eopFile->Initialize();
//    ItrfCoefficientsFile* itrfFile = new ItrfCoefficientsFile(nutFileName, planFileName);
//    itrfFile->Initialize();
//    TimeConverterUtil::SetLeapSecsFileReader(lsFile);
//    TimeConverterUtil::SetEopFile(eopFile);

   // for SolarSystem, internal CoordinateSystem
   SolarSystem *ssPtr = new SolarSystem("MySolarSystem");
   
   // set J2000Body for Earth
   CelestialBody *earthPtr = ssPtr->GetBody("Earth");
   std::string j2000BodyName = "Earth";
   CelestialBody *j2000Body = earthPtr;
   earthPtr->SetJ2000BodyName(j2000BodyName);
   earthPtr->SetJ2000Body(j2000Body);
   
   // for CoordinateSystem - EarthMJ2000Eq
   CoordinateSystem *csPtr = new CoordinateSystem("CoordinateSystem", "EarthMJ2000Eq");
   AxisSystem *mj2000EqAxis = new MJ2000EqAxes("MJ2000Eq");
   csPtr->SetRefObject(mj2000EqAxis, Gmat::AXIS_SYSTEM, mj2000EqAxis->GetName());
   csPtr->SetSolarSystem(ssPtr);
   csPtr->SetStringParameter("Origin", "Earth");
   csPtr->SetStringParameter("J2000Body", "Earth");
   csPtr->SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   csPtr->Initialize();
   
//    // for CoordinateSystem - EarthMJ2000Ec
//    CoordinateSystem *eccsPtr = new CoordinateSystem("CoordinateSystem", "EarthMJ2000Ec");
//    AxisSystem *ecAxis = new MJ2000EcAxes("MJ2000Ec");
//    eccsPtr->SetRefObject(ecAxis, Gmat::AXIS_SYSTEM, ecAxis->GetName());
//    eccsPtr->SetSolarSystem(ssPtr);
//    eccsPtr->SetStringParameter("Origin", "Earth");
//    eccsPtr->SetStringParameter("J2000Body", "Earth");
//    eccsPtr->SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
//    eccsPtr->Initialize();
   
   // set SLP file to SolarSystem
   std::string slpFileName = "C:\\projects\\gmat\\files\\planetary_ephem\\slp\\mn2000.pc";
   SlpFile *theSlpFile = new SlpFile(slpFileName);
   ssPtr->SetSource(Gmat::SLP);
   ssPtr->SetSourceFile(theSlpFile);
   
   // for Spacecraft
   Spacecraft *scPtr = new Spacecraft("MySpacecraft");
   scPtr->SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM);
   
   // Create Burn
   Burn *impBurnPtr = new ImpulsiveBurn("MyBurn");
   
   out.Put("***************************** Set Burn Elements to 0.1, 0.2, 0.3");
   impBurnPtr->SetRealParameter(impBurnPtr->GetParameterID("Element1"), 0.1);
   impBurnPtr->SetRealParameter(impBurnPtr->GetParameterID("Element2"), 0.2);
   impBurnPtr->SetRealParameter(impBurnPtr->GetParameterID("Element3"), 0.3);

   
   //---------------------------------------------------------------------------
   out.Put("======================================== test BurnParameters\n");
   //---------------------------------------------------------------------------

   out.Put("============================== test DeltaVDir1()");
   DeltaVDir1 *dv1Param = new DeltaVDir1();
   dv1Param->SetSolarSystem(ssPtr);
   dv1Param->SetInternalCoordSystem(csPtr);
   dv1Param->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   dv1Param->SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   
   out.Put("----- test AddRefObject(impBurnPtr)");
   dv1Param->AddRefObject(impBurnPtr);
   out.Put("num Refobjects = ", dv1Param->GetNumRefObjects());
   
   out.Put("----- test dv1Param->EvaluateReal()");
   realVal = dv1Param->EvaluateReal();  
   out.Validate(realVal, 0.1);
   
   out.Put("============================== test DeltaVDir2()");
   DeltaVDir2 *dv2Param = new DeltaVDir2();
   dv2Param->SetSolarSystem(ssPtr);
   dv2Param->SetInternalCoordSystem(csPtr);
   dv2Param->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   dv2Param->SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   
   out.Put("----- test AddRefObject(impBurnPtr)");
   dv2Param->AddRefObject(impBurnPtr);
   out.Put("num objects = ", dv2Param->GetNumRefObjects());
   
   out.Put("----- test dv2Param->EvaluateReal()");
   realVal = dv2Param->EvaluateReal();  
   out.Validate(realVal, 0.2);
   
   out.Put("============================== test DeltaVDir3()");
   DeltaVDir3 *dv3Param = new DeltaVDir3();
   dv3Param->SetSolarSystem(ssPtr);
   dv3Param->SetInternalCoordSystem(csPtr);
   dv3Param->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   dv3Param->SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   
   out.Put("----- test AddRefObject(impBurnPtr)");
   dv3Param->AddRefObject(impBurnPtr);
   out.Put("num objects = ", dv3Param->GetNumRefObjects());
   
   out.Put("----- test dv3Param->EvaluateReal()");
   realVal = dv3Param->EvaluateReal();  
   out.Validate(realVal, 0.3);
   
   //---------------------------------------------
   // clean up
   //---------------------------------------------
   delete dv1Param;
   delete dv2Param;
   delete dv3Param;
   delete impBurnPtr;
   delete scPtr;
   delete ssPtr;
   //delete theSlpFile; //problem deleting!!
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   TestOutput out("..\\..\\Test\\TestParam\\TestBurnParamOut.txt");
   out.SetPrecision(12);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of BurnParameters!!");
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
