//------------------------------------------------------------------------------
//                                TestImpulsiveBurn
//------------------------------------------------------------------------------
// GMAT: Goddard Mission Analysis Tool
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
// Created: 2009.02.27
//
/**
 * Purpose:
 *   Unit test driver of ImpulsiveBurn class.
 *
 * Output file:
 *   TestImpulsiveBurnOut.txt
 *
 * Description of ImpulsiveBurn:
 *   The ImpulsiveBurn class allows the spacecraft to undergo an instantaneous
 *   delta V maneuver.  The user can configure the ImpulsiveBurn by defining its
 *   origin, coordiante system type, axes type, and magnitude of the thrust vectors.
 * 
 * Test Procedure:
 *   1. Create FileManager and read gmat_startup_file.txt
 *   2. Create SolarSystem.
 *   3. Create FuelTank and set initial FuelMass.
 *   4. Create Spacecraft and set initial DryMass.
 *   5. Set FuelTank to Spacecraft.
 *   6. Create ImpulsiveBurn and set initial values.
 *   7. Set FuelTank to ImpulsiveBurn.
 *   8. Set SolarSystem to ImpulsiveBurn and initialize.
 *   9. Set the Spacecraft to ImpulsiveBurn to maneuver.
 *   10. Call Fire() on ImpulsiveBurn.
 *   11. Check if tank mass was decreased by expected amount.
 *
 * Validation method:
 *   The test driver code knows expected results and throws an exception if the
 *   result is not within the tolerance.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "SolarSystem.hpp"
#include "FuelTank.hpp"
#include "ImpulsiveBurn.hpp"
#include "Spacecraft.hpp"
#include "TestOutput.hpp"
#include "BaseException.hpp"
#include "MessageInterface.hpp"
#include "ConsoleMessageReceiver.hpp"
#include "FileManager.hpp"

#include <iostream>

using namespace std;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   out.Put("======================================== test ImpulsiveBurn\n");
   
   Integer id;
   
   // Create SolarSystem
   SolarSystem *ss = new SolarSystem("SolarSystem");
   
   // Create FuelTank
   FuelTank *tank1 = new FuelTank("Tank1");
   id = tank1->GetParameterID("FuelMass");
   tank1->SetRealParameter(id, 75.0);
   
   // Create Spacecraft
   Spacecraft *sat1 = new Spacecraft("Sat1");
   sat1->SetRefObject(ss->GetBody("Earth"), Gmat::SPACE_POINT); // Set J2000Body
   sat1->SetRealParameter(id, 1000.0);
   id = sat1->GetParameterID("DryMass");
   sat1->SetRealParameter(id, 1000.0);
   
   // Assign clone of Tank to Spacecraft since Spacecraft is expecting cloned one
   sat1->SetStringParameter("Tanks", "Tank1");
   GmatBase *clonedTank1 = tank1->Clone();
   sat1->SetRefObject(clonedTank1, Gmat::HARDWARE, "Tank1");
   
   // Create ImpulsiveBurn
   ImpulsiveBurn *impBurn1 = new ImpulsiveBurn("ImpBurn1");
   id = impBurn1->GetParameterID("Axes");
   impBurn1->SetStringParameter(id, "VNB");
   id = impBurn1->GetParameterID("Element1");
   impBurn1->SetRealParameter(id, 0.1);
   id = impBurn1->GetParameterID("Isp");
   impBurn1->SetRealParameter(id, 300.0);
   id = impBurn1->GetParameterID("GravitationalAccel");
   impBurn1->SetRealParameter(id, 9.80665);
   id = impBurn1->GetParameterID("DecrementMass");
   impBurn1->SetBooleanParameter(id, true);
   
   // Assign Tank to ImpulsiveBurn
   id = impBurn1->GetParameterID("Tank");
   impBurn1->SetStringParameter(id, "Tank1");
   
   try
   {
      // Set SolarSystem to ImpulsiveBurn and Initialize
      impBurn1->SetSolarSystem(ss);
      impBurn1->Initialize();
      
      // Set Spacecraft to ImpulsiveBurn and fire
      impBurn1->SetSpacecraftToManeuver(sat1);
      impBurn1->Fire();
      
      // Get Total tank mass after fire
      id = tank1->GetParameterID("FuelMass");
      Real tankMass = tank1->GetRealParameter(id);
      out.Put("original tank Mass = ", tankMass);
      
      Real clonedTankMass = clonedTank1->GetRealParameter(id);
      out.Put("cloned tank Mass   = ", clonedTankMass);
      out.Validate(39.0742, clonedTankMass);
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   try
   {
      // Test setting any tank here
      impBurn1->SetRefObject(tank1, Gmat::FUEL_TANK, "Tank1");
      
      // Set Spacecraft to ImpulsiveBurn and fire
      impBurn1->SetSpacecraftToManeuver(sat1);
      impBurn1->Fire();
      
      // Get Total tank mass after fire
      id = tank1->GetParameterID("FuelMass");
      Real tankMass = tank1->GetRealParameter(id);
      out.Put("original tank Mass = ", tankMass);
      
      out.Put("===== Testing GetRefObjectNameArray()");
      StringArray refObjNames = impBurn1->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
      for (unsigned int i=0; i<refObjNames.size(); i++)
         out.Put("   ", refObjNames[i]);
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
   }
   
   delete impBurn1;
   delete ss;
   delete sat1;
   delete tank1;
   
   return 1;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   std::string startupFile = "gmat_startup_file.txt";
   FileManager *fm = FileManager::Instance();
   fm->ReadStartupFile(startupFile);
   
   ConsoleMessageReceiver *consoleMsg = ConsoleMessageReceiver::Instance();
   MessageInterface::SetMessageReceiver(consoleMsg);
   MessageInterface::SetLogFile("../../TestBurn/GmatLog.txt");
   
   TestOutput out("../../TestBurn/TestImpulsiveBurnOut.txt");
   out.Put(GmatTimeUtil::GetCurrentTime());
   
   try
   {
      RunTest(out);
      out.Put("\nSuccessfully ran unit testing of ImpulsiveBurn!!");
   }
   catch (BaseException &e)
   {
      out.Put(e.GetFullMessage());
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
