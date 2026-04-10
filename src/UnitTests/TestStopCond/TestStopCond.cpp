//------------------------------------------------------------------------------
//                                  TestStopCond
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
/**
 * Test driver for StopCond and subclasses.
 */
//------------------------------------------------------------------------------
#include <iostream>
#include "gmatdefs.hpp"
#include "StopConditionException.hpp"
#include "StopCondition.hpp"

#include "TimeParameters.hpp"
#include "CartesianParameters.hpp"
#include "OrbitalParameters.hpp"

#include "Interpolator.hpp"
#include "LinearInterpolator.hpp"

#include "Spacecraft.hpp"
#include "SolarSystem.hpp"

// coordinate system
#include "CoordinateSystem.hpp"
#include "MJ2000EqAxes.hpp"

// files
#include "SlpFile.hpp"

#include "TestOutput.hpp"

using namespace std;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   Real tol = 0.000001;
   
   // set J2000Body for Earth
   SolarSystem *ssPtr = new SolarSystem();
   CelestialBody *earthPtr = ssPtr->GetBody("Earth");
   std::string j2000BodyName = "Earth";
   CelestialBody *j2000Body = earthPtr;
   earthPtr->SetJ2000BodyName(j2000BodyName);
   earthPtr->SetJ2000Body(j2000Body);
   
   // set J2000Body for Sun
   CelestialBody *sunPtr = ssPtr->GetBody("Sun");
   sunPtr->SetJ2000BodyName("Earth");
   sunPtr->SetJ2000Body(j2000Body);
   
   // for CoordinateSystem
   CoordinateSystem *csPtr = new CoordinateSystem("CoordinateSystem", "EarthMJ2000Eq");
   AxisSystem *mj2000EqAxis = new MJ2000EqAxes("MJ2000Eq");
   csPtr->SetRefObject(mj2000EqAxis, Gmat::AXIS_SYSTEM, mj2000EqAxis->GetName());
   csPtr->SetStringParameter("Origin", "Earth");
   csPtr->SetStringParameter("J2000Body", "Earth");
   csPtr->SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   
   // set SLP file to SolarSystem
   std::string slpFileName = "C:/projects/gmat/files/planetary_ephem/slp/mn2000.pc";
   SlpFile *theSlpFile = new SlpFile(slpFileName);
   ssPtr->SetSource(Gmat::SLP);
   ssPtr->SetSourceFile(theSlpFile);
   
   // for Spacecraft
   Spacecraft *scPtr = new Spacecraft();
   scPtr->SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM);
   
   CurrA1MJD *timeParam = new CurrA1MJD("CurrentTime", scPtr);
   out.Put("CurrentTime = ", timeParam->EvaluateReal());
   
   out.Put("***************************** test StopCondition\n");
   out.Put("NOTE: All results are dependent on Spacecraft defaults\n");
   
   
   //---------------------------------------------------------------------------
   out.Put("\n======================================== test Stop on CurrA1MJD\n");
   //---------------------------------------------------------------------------
   CurrA1MJD *currA1MJDParam = new CurrA1MJD("CurrA1MJD", scPtr);
   StopCondition ctStop = StopCondition("CurrA1MJDStop", "", timeParam, currA1MJDParam);
   ctStop.SetEpochParameter(timeParam);
   ctStop.Initialize();
   
   out.Put("---------- currA1MJDParam->EvaluateReal() should return 21545.0");
   out.Validate(currA1MJDParam->EvaluateReal(), 21545.0);
   ctStop.SetStringParameter("Goal", "21546.0");
   out.Put("---------- Evaluate() should return false");
   
   try
   {
      out.Validate(ctStop.Evaluate(), false);
   }
   catch (StopConditionException &e)
   {
       out.Put("<<<<<<<<<< ", e.GetMessage());
       out.Put("Hit enter to continue\n");
       cin.get();
   }
   
   ctStop.SetStringParameter("Goal", "21543.0");
   out.Put("---------- Evaluate() should return true");
   out.Validate(ctStop.Evaluate(), true);
   
   //---------------------------------------------------------------------------
   out.Put("\n======================================== test Stop on ElapsedDays\n");
   //---------------------------------------------------------------------------
   ElapsedDays *stopEdParam1 = new ElapsedDays("stopEdParam1", scPtr);
   stopEdParam1->SetRealParameter("InitialEpoch", 21500.0);
   out.Put("stopEdParam1->EvaluateReal() = ", stopEdParam1->EvaluateReal());
   
   StopCondition edStop1 = StopCondition("ElapsedDaysStop", "", timeParam, stopEdParam1,
                                        100.0, tol);
   
   out.Put("---------- Evaluate() should return false because 45 < 100 (goal)");
   out.Validate(edStop1.Evaluate(), false);
   
   edStop1.SetStringParameter("Goal", "10.0");
   out.Put("---------- Evaluate() should return true because 45 > 10 (goal)");
   out.Validate(edStop1.Evaluate(), true);
   
   //---------------------------------------------------------------------------
   out.Put("\n======================================== test Stop on CartXParam\n");
   //---------------------------------------------------------------------------
   Real xGoal = 7200.0;
   CartX *cartXParam = new CartX("CartX", scPtr);
   cartXParam->SetSolarSystem(ssPtr);
   cartXParam->SetInternalCoordSystem(csPtr);
   cartXParam->SetRefObjectName(Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartXParam->SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   cartXParam->Initialize();
   
   StopCondition posXStop = StopCondition("XPositionStop", "", timeParam, cartXParam,
                                          xGoal, tol);
   out.Put("---------- cartXParam->EvaluateReal() should return 7100.0");
   try
   {
      out.Validate(cartXParam->EvaluateReal(), 7100.0);
   }
   catch (StopConditionException &e)
   {
       out.Put("<<<<<<<<<< ", e.GetMessage());
   }
   
   out.Put("----- Interpolator is not set");
   out.Put("valid = ", posXStop.Validate());
   out.Put("buffer size = ", posXStop.GetBufferSize());
   
   out.Put("----- Now set Interpolator");
   Interpolator *interp = new LinearInterpolator();
   try
   {
       posXStop.SetInterpolator(interp);
   }
   catch (StopConditionException &e)
   {
       out.Put("<<<<<<<<<< ", e.GetMessage());
   }
   
   out.Put("----- Now initialize StopCondition");
   posXStop.Initialize();
   out.Put("valid = ", posXStop.Validate());
   out.Put("buffer size = ", posXStop.GetBufferSize());
   out.Put("---------- Evaluate() should return false");
   try
   {
      out.Validate(posXStop.Evaluate(), false);
   }
   catch (StopConditionException &e)
   {
       out.Put("<<<<<<<<<< ", e.GetMessage());
   }
   
   //---------------------------------------------------------------------------
   out.Put("\n======================================== test copy constructor\n");
   //---------------------------------------------------------------------------
      
   out.Put("---------- test StopCondition() - stop on CartXParam");
   StopCondition xStop1 = StopCondition(posXStop);
   out.Put("buffer size = ", xStop1.GetBufferSize());
   out.Put("---------- Validate() should return true");
   out.Validate(xStop1.Validate(), true);
   
   xStop1.Initialize();
   
   out.Put("---------- Evaluate() should return false");
   try
   {
      out.Validate(xStop1.Evaluate(), false);
   }
   catch (StopConditionException &e)
   {
       out.Put("<<<<<<<<<< ", e.GetMessage());
   }
   
   //---------------------------------------------------------------------------
   out.Put("\n======================================== test Stop on Apoapsis\n");
   //---------------------------------------------------------------------------
   StopCondition apoapsisStop = StopCondition("StopOnApoapsis");
   apoapsisStop.SetSolarSystem(ssPtr);
   
   out.Put("---------- Initialize() should throw an exception - because StopParam is not set");
   try
   {
      out.Put(apoapsisStop.Initialize());
   }
   catch (StopConditionException &e)
   {
       out.Put("<<<<<<<<<< ", e.GetMessage());
   }
   
   out.Put("---------- apoapsisStop.SetStopParameter(apoapsisParam)");
   Apoapsis *apoapsisParam = new Apoapsis("apoapsis", scPtr);
   apoapsisParam->SetSolarSystem(ssPtr);
   apoapsisParam->SetInternalCoordSystem(csPtr);
   apoapsisParam->SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   apoapsisParam->SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   apoapsisParam->Initialize();
   
   apoapsisStop.SetStopParameter(apoapsisParam);
   apoapsisStop.Initialize();
   
   out.Put("---------- Validate() should return true");
   out.Validate(apoapsisStop.Validate(), true);
   
   out.Put("---------- Evaluate() should return false");
   
   try
   {
       out.Validate(apoapsisStop.Evaluate(), false);
   }
   catch (StopConditionException &e)
   {
       out.Put("<<<<<<<<<< ", e.GetMessage());
   }

   
   //---------------------------------------------------------------------------
   out.Put("\n======================================== test Stop on Periapsis\n");
   //---------------------------------------------------------------------------
   StopCondition periapsisStop = StopCondition("StopOnPeriapsis");
   periapsisStop.SetSolarSystem(ssPtr);
      
   out.Put("---------- Validate() should throw an exception - because StopParam is not set");
   try
   {
      out.Put(periapsisStop.Validate());
   }
   catch (StopConditionException &e)
   {
       out.Put("<<<<<<<<<< ", e.GetMessage());
   }

   out.Put("---------- periapsisStop.SetEpochParameter(currA1MJDParam)");
   periapsisStop.SetEpochParameter(currA1MJDParam);
   
   out.Put("---------- periapsisStop.SetStopParameter(periapsisParam)");
   Periapsis *periapsisParam = new Periapsis("periapsis", scPtr);
   periapsisParam->SetSolarSystem(ssPtr);
   
   periapsisParam->SetInternalCoordSystem(csPtr);
   periapsisParam->SetRefObject(csPtr, Gmat::COORDINATE_SYSTEM, "EarthMJ2000Eq");
   periapsisParam->SetRefObject(earthPtr, Gmat::SPACE_POINT, "Earth");
   periapsisParam->Initialize();
   
   periapsisStop.SetStopParameter(periapsisParam);
   periapsisStop.Initialize();
   
   out.Put("---------- Validate() should return true");
   out.Validate(periapsisStop.Validate(), true);
   
   out.Put("---------- Evaluate() should return false");

   try
   {
       out.Validate(periapsisStop.Evaluate(), false);
   }
   catch (StopConditionException &e)
   {
       out.Put("<<<<<<<<<< ", e.GetMessage());
   }
   
   //---------------------------------------------------------------------------
   out.Put("\n======================================== test Stop on goal variable\n");
   //---------------------------------------------------------------------------
   ElapsedDays *stopEdParam2 = new ElapsedDays("stopEdParam2", scPtr);
   stopEdParam2->SetRealParameter("InitialEpoch", 21546.0);
   out.Put("---------- stopEdParam2 should return -1");
   out.Validate(stopEdParam2->EvaluateReal(), -1.0);
   
   ElapsedDays *goalEdParam = new ElapsedDays("goalEdParam", scPtr);
   out.Put("---------- Set InitialEpoch of stopEdParam2 to 21546");
   goalEdParam->SetRealParameter("InitialEpoch", 21544.0);
   out.Put("---------- goalEdParams should return 1.0");
   out.Validate(goalEdParam->EvaluateReal(), 1.0);
   
   StopCondition edStop2 = StopCondition("edStop2", "", timeParam, stopEdParam2);
   
   out.Put("---------- Set goalEdParam as Goal string");
   try
   {
      edStop2.SetStringParameter("Goal", "goalEdParam");
      
      out.Put("---------- Initialize() should throw an exception because goal param is not set");
      edStop2.Initialize();
   }
   catch(StopConditionException &e)
   {
      out.Put("<<<<<<<<<< ", e.GetMessage());
   }
   
   out.Put("---------- Now set ref object");
   StringArray refObjNames = edStop2.GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
   out.Put("refObjNames.size() = ", (int)refObjNames.size());
   
   try
   {
      for (unsigned int i=0; i<refObjNames.size(); i++)
      {
         out.Put(refObjNames[i]);
         if (refObjNames[i] == "stopEdParam2")
            edStop2.SetRefObject(stopEdParam2, Gmat::PARAMETER, refObjNames[i]);
         else if (refObjNames[i] == "goalEdParam")
            edStop2.SetRefObject(goalEdParam, Gmat::PARAMETER, refObjNames[i]);
      }
   }
   catch(StopConditionException &e)
   {
      out.Put("<<<<<<<<<< ", e.GetMessage());
   }
   
   out.Put("---------- Evaluate() should return false");
   out.Validate(edStop2.Evaluate(), false);
   
   out.Put("---------- Set InitialEpoch of stopEdParam2 to 21543");
   stopEdParam2->SetRealParameter("InitialEpoch", 21543.0);
   out.Put("---------- stopEdParam2 should return 2.0");
   out.Validate(stopEdParam2->EvaluateReal(), 2.0);
   
   out.Put("---------- Evaluate() should return true");
   out.Validate(edStop2.Evaluate(), true);
   

   // Clean Up
   delete timeParam;
   delete currA1MJDParam;
   delete stopEdParam1;
   delete cartXParam;
   delete interp;
   delete apoapsisParam;
   delete periapsisParam;
   delete stopEdParam2;
   delete goalEdParam;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   TestOutput out("../../Test/TestStopCond/TestStopCondOut.txt");
   out.SetPrecision(16);
   out.SetWidth(20);
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of StopCondition!!");
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
