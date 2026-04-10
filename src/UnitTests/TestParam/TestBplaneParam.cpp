//------------------------------------------------------------------------------
//                                  TestBplaneParam
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
 * Test driver for B-Plane parameters.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "BplaneParameters.hpp"
#include "ImpulsiveBurn.hpp"
#include "SolarSystem.hpp"

// coordinate system
#include "CoordinateSystem.hpp"
#include "MJ2000EqAxes.hpp"
#include "MJ2000EcAxes.hpp"

// files
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
   
   // for CoordinateSystem - EarthMJ2000Ec
   CoordinateSystem *eccsPtr = new CoordinateSystem("CoordinateSystem", "EarthMJ2000Ec");
   AxisSystem *ecAxis = new MJ2000EcAxes("MJ2000Ec");
   eccsPtr->SetRefObject(ecAxis, Gmat::AXIS_SYSTEM, ecAxis->GetName());
   eccsPtr->SetSolarSystem(ssPtr);
   eccsPtr->SetStringParameter("Origin", "Earth");
   eccsPtr->SetStringParameter("J2000Body", "Earth");
   eccsPtr->SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   eccsPtr->Initialize();
   
   // set SLP file to SolarSystem
   std::string slpFileName = "C:\\projects\\gmat\\files\\planetary_ephem\\slp\\mn2000.pc";
   SlpFile *theSlpFile = new SlpFile(slpFileName);
   ssPtr->SetSource(Gmat::SLP);
   ssPtr->SetSourceFile(theSlpFile);
   
   // for Spacecraft
   Spacecraft *scPtr = new Spacecraft("MySpacecraft");
   scPtr->SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM);
   
   //---------------------------------------------------------------------------
   out.Put("======================================== test BplaneParameters\n");
   //---------------------------------------------------------------------------
   
   out.Put("=================================== Test with default spacecraft orbit");
   out.Put("========== state = ", scPtr->GetCartesianState().ToString());
   
   out.Put("============================== test BdotT()");
   BdotT *btParam = new BdotT();
   btParam->SetSolarSystem(ssPtr);
   btParam->SetInternalCoordSystem(csPtr);
   btParam->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   btParam->SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   btParam->SetRefObjectName(Gmat::SPACECRAFT, "MySpacecraft");
   btParam->SetRefObject(scPtr, Gmat::SPACECRAFT, "MySpacecraft");
   btParam->Initialize();
   
   out.Put("num RefObjects = ", btParam->GetNumRefObjects());
   out.Put("----- test btParam->EvaluateReal()");
   out.Put("----- Should get an exception due to non-hyperbolic orbit");
   realVal = btParam->EvaluateReal();
   
   out.Put("============================== test BdotR()");
   BdotR *brParam = new BdotR();
   brParam->SetSolarSystem(ssPtr);
   brParam->SetInternalCoordSystem(csPtr);
   brParam->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   brParam->SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   brParam->SetRefObjectName(Gmat::SPACECRAFT, "MySpacecraft");
   brParam->SetRefObject(scPtr, Gmat::SPACECRAFT, "MySpacecraft");
   brParam->Initialize();
   
   out.Put("num RefObjects = ", brParam->GetNumRefObjects());
   out.Put("----- test brParam->EvaluateReal()");
   out.Put("----- Should get an exception due to non-hyperbolic orbit");
   realVal = brParam->EvaluateReal();
   out.Put("");

   out.Put("=================================== Test in EarthMJ2000Ec");
   
   BdotT *btecParam = new BdotT();
   btecParam->SetSolarSystem(ssPtr);
   btecParam->SetInternalCoordSystem(csPtr);
   btecParam->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Ec");
   btecParam->SetRefObject(eccsPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Ec");
   btecParam->SetRefObjectName(Gmat::SPACECRAFT, "MySpacecraft");
   btecParam->SetRefObject(scPtr, Gmat::SPACECRAFT, "MySpacecraft");
   
   out.Put("----- test btParam->EvaluateReal()");
   out.Put("----- Should get an exception due to non-hyperbolic orbit");
   realVal = btecParam->EvaluateReal();
   out.Put("");
   
   BdotR *brecParam = new BdotR();
   brecParam->SetSolarSystem(ssPtr);
   brecParam->SetInternalCoordSystem(csPtr);
   brecParam->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Ec");
   brecParam->SetRefObject(eccsPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Ec");
   brecParam->SetRefObjectName(Gmat::SPACECRAFT, "MySpacecraft");
   brecParam->SetRefObject(scPtr, Gmat::SPACECRAFT, "MySpacecraft");
   
   out.Put("----- test brParam->EvaluateReal()");
   out.Put("----- Should get an exception due to non-hyperbolic orbit");
   realVal = brecParam->EvaluateReal();
   out.Put("");
   
   /*
   ***************************************************************************
   * From Steve Hughes(2005/06/16)
   % Position and velocity in EarthMJ2000Eq
   % (Epoch does not matter so you can choose your own)
   rv = [233410.6846140172000   83651.0868276347170  -168884.42195943173]';
   vv = [-0.4038280708568842      2.0665425988121107  0.4654706868112324]';

   *%  Bplane Coordinates for EarthMJ2000Eq system*
   Sat.EarthMJ2000Eq.BdotT =   365738.686341826
   Sat.EarthMJ2000Eq.BdotR =   276107.260600374
   
   *%  Bplane Coordinates for EarthMJ2000Ec system*
   Sat.EarthMJ2000Ec.BdotT =   381942.623061352
   Sat.EarthMJ2000Ec.BdotR =   253218.95413318
   ***************************************************************************
   */
   
   // Now set spacecraft state to hyperbolic orbit
   Real state[6];
 
   state[0] =  233410.6846140172000;
   state[1] =   83651.0868276347170;
   state[2] = -168884.42195943173;
   state[3] = -0.4038280708568842;
   state[4] =  2.0665425988121107;
   state[5] =  0.4654706868112324;
   scPtr->SetState("Cartesian", state);
   Rvector6 stateVec(state);
   
   out.Put("=================================== Test with hyperbolic orbit");
   out.Put("========== state = ", scPtr->GetCartesianState().ToString());
   
   // Now test in EartMJ2000Eq
   
   out.Put("=================================== Test in EarthMJ2000Eq");
   btParam->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   btParam->SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   
   expResult = 365738.686341826;
   out.Put("----- test btParam->EvaluateReal() Should return ", expResult);
   realVal = btParam->EvaluateReal();
   out.Validate(realVal, expResult);
   
   expResult = 276107.260600374;
   out.Put("----- test brParam->EvaluateReal() Should return ", expResult);
   realVal = brParam->EvaluateReal();
   out.Validate(realVal, expResult);
   
   // Now test in EartMJ2000Ec
   
   out.Put("=================================== Test in EarthMJ2000Ec");
      
   realVal = btecParam->EvaluateReal();
   
   out.Put("----- test btecParam->EvaluateReal() Should return ", expResult);
   expResult = 381942.623061352;
   out.Validate(realVal, expResult);
   
   expResult = 253218.95413318;
   out.Put("----- test brecParam->EvaluateReal() Should return ", expResult);
   realVal = brecParam->EvaluateReal();
   out.Validate(realVal, expResult);
   
   //---------------------------------------------
   // clean up
   //---------------------------------------------
   delete btParam;
   delete brParam;
   delete btecParam;
   delete brecParam;
   delete scPtr;
   delete ssPtr;
   delete csPtr;
   //delete theSlpFile; //problem deleting!!
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   TestOutput out("..\\..\\Test\\TestParam\\TestBplaneParamOut.txt");
   out.SetPrecision(12);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of BplaneParameters!!");
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
