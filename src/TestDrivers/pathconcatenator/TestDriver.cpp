//------------------------------------------------------------------------------
//                           GmatConsole driver
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Darrel J. Conway
// Created: 2003/08/28
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Program entry point for GmatConsole.
 */
//------------------------------------------------------------------------------

#include "TestDriver.hpp"

// GMAT code
#include "gmatdefs.hpp"
#include "Moderator.hpp"
#include "Spacecraft.hpp"
#include "ODEModel.hpp"
#include "SolarSystem.hpp"
#include "PointMassForce.hpp"
#include "SolarRadiationPressure.hpp"


// CSALT Interface code
#include "GmatPathConcatenator.hpp"
#include "DynamicsConfiguration.hpp"
#include "FunctionInputData.hpp"
#include "PathFunctionContainer.hpp"
#include "ConsoleMessageReceiver.hpp"


// This macro, when defined, turns on the MessageInterface used in GMAT debugging
#define SET_DEBUG_MODE


#include <fstream>

// // Specific classes used in the tests
// #include "ComponentTester.hpp"

#ifdef DEBUG_CONSOLE
#include <unistd.h>
#endif


/**
 * The program entry point.
 * 
 * @param argc The count of the input arguments.
 * @param argv The input arguments.
 *
 * @note: argc and argv are not used for this test setup.
 * 
 * @return 0 on success.
 */
int main(int argc, char *argv[])
{
   int retval = 0; 

   std::string msg = "General Mission Analysis Tool\nComponent Test Program\n";

   msg += "Build Date: ";
   msg += __DATE__;
   msg += "  ";
   msg += __TIME__;
   
   std::cout << "\n********************************************\n"
             << "***  GMAT/CSALT Unit Test Application\n"
             << "********************************************\n\n"
             << msg << "\n\n"
             << std::endl;
   
   bool        testMode = true;

   // Setup the Moderator (before activating messages to reduce chatter!)
   Moderator *mod = Moderator::Instance();
   mod->Initialize("../../../../application/bin/gmat_startup_file.txt");

   #ifdef SET_DEBUG_MODE
      // Hook up GMAT messages so that we see active debug
      ConsoleMessageReceiver *theMessageReceiver =
            ConsoleMessageReceiver::Instance();
      MessageInterface::SetMessageReceiver(theMessageReceiver);
   #endif

   if (!RunUnitTests())
      retval = -1;
      
   return retval;
}


/**
 * Run the tests for this problem collection.  If there is a lot to do, this
 * should call into other code (probably in external files).
 */
bool RunUnitTests()
{
   bool retval = true;

   // Grab the Moderator initialzed in main()
   Moderator *mod = Moderator::Instance();

   try
   {
      SolarSystem *ss = mod->GetDefaultSolarSystem();
      // Cannot do this here: deafult cs's are built in CreateSpacecraft (GMT-6200)
      //CoordinateSystem *cs = mod->GetCoordinateSystem("EarthMJ2000Eq");
      std::cout << "SolarSys: " << ss << ", Default Coord sys: ";

      // Pieces that GMAT would script
      Spacecraft *sc = (Spacecraft*)mod->CreateSpacecraft("Spacecraft", "SeaSalty");
//      Spacecraft *sc2 = (Spacecraft*)mod->CreateSpacecraft("Spacecraft", "SeaSaltier");

      CoordinateSystem *cs = mod->GetCoordinateSystem("EarthMJ2000Eq");
      sc->SetRefObject(cs, cs->GetType(), cs->GetName());
//      sc2->SetRefObject(cs, cs->GetType(), cs->GetName());

      std::cout  << cs;
      if (cs)
         std::cout  << "Default CoordinateSystem used is " << cs->GetName();
      std::cout << "\n";

      sc->SetSolarSystem(ss);
      std::cout << "\nCreated Spacecraft named " << sc->GetName() << "\n";

//      sc2->SetSolarSystem(ss);
//      std::cout << "\nCreated Spacecraft named " << sc2->GetName() << "\n";

      // The force model: Earth, Moon, SRP
      ODEModel *ode = new ODEModel("CsaltDynamics");
      ode->SetSolarSystem(ss);
      PointMassForce *epm = new PointMassForce("EarthPointMass");
      PointMassForce *lpm = new PointMassForce("MoonPointMass");
      SolarRadiationPressure *srp = new SolarRadiationPressure("SRP");
      lpm->SetStringParameter("BodyName", "Luna");
      ode->AddForce(epm);
      ode->AddForce(lpm);
      ode->AddForce(srp);
      std::cout << "Created ForceModel (ODEModel) named " << ode->GetName() << "\n";

//      ODEModel *ode2 = new ODEModel("MoreCsaltDynamics");
//      ode2->SetSolarSystem(ss);
//      PointMassForce *epm2 = new PointMassForce("EarthPointMass");
//      ode2->AddForce(epm2);
//      std::cout << "Created ForceModel (ODEModel) named " << ode2->GetName() << "\n";

      // @todo  Add a transient force (thrust)

      // Component that is embedded in the concatenator, so we are testing this
      // puppy too.
      DynamicsConfiguration *dc = new DynamicsConfiguration("TrialDynamics");
      dc->SetSolarSystem(ss);
      dc->SetInternalCoordSystem(cs);
      std::cout << "Created DynamicsConfig named " << dc->GetName() << "\n";

      std::cout << "Setting up the dynamics:\n";

      dc->SetStringParameter("DynamicsModels", ode->GetName());
//      dc->SetStringParameter("DynamicsModels", ode2->GetName());
      dc->SetStringParameter("Spacecraft", sc->GetName());
//      dc->SetStringParameter("Spacecraft", sc2->GetName());

      std::cout << "\n"
                << dc->GetGeneratingString(Gmat::NO_COMMENTS, "   ")
                << "\n";

      // The concatenator tested here
      GmatPathConcatenator *gpc = new GmatPathConcatenator();
      std::cout << "Created GmatPathConcatenator at address " << gpc << "\n\n";

      // Now let's play GMAT:
      // Mock up Sandbox initialization
      std::cout << "********************************************************\n"
                << "\"Sandbox\" Initialization Starting\n";
      StringArray refs = dc->GetRefObjectNameArray(Gmat::UNKNOWN_OBJECT);
      for (UnsignedInt i = 0; i < refs.size(); ++i)
      {
         if (refs[i] == ode->GetName())
            dc->SetRefObject(ode, ode->GetType(), ode->GetName());
//         if (refs[i] == ode2->GetName())
//            dc->SetRefObject(ode2, ode2->GetType(), ode2->GetName());
         else if (refs[i] == sc->GetName())
            dc->SetRefObject(sc, sc->GetType(), sc->GetName());
//         else if (refs[i] == sc2->GetName())
//            dc->SetRefObject(sc2, sc2->GetType(), sc2->GetName());
         else
            std::cout << dc->GetName() << " references "
                      << refs[i] << " but the pointer was not set\n";
      }

      bool status, allStatus = true;

      status  = sc->Initialize();
//      status &= sc2->Initialize();
      allStatus &= status;
      if (!status)
         std::cout << "   Spacecraft initialization failed\n";

      status = cs->Initialize();
      allStatus &= status;
      if (!status)
         std::cout << "   Coordinate System initialization failed\n";

      status = dc->Initialize();
      allStatus &= status;
      if (!status)
         std::cout << "   DynamicsConfiguration initialization failed\n";

      status = ode->Initialize();
      allStatus &= status;
      if (!status)
         std::cout << "   ODE model initialization failed\n";

      std::cout << "\nObject initialization "
                << (allStatus ? "succeeded" : "failed") << "\n\n";


      std::cout << "********************************************************\n"
                << "\"Sandbox\" Execution Starting\n";

      status = dc->PrepareToOptimize();
      if (status)
         std::cout << "DynamicsConfig reports it is ready to optimize\n";

      // Interface needs a control input, but there isn't one for this check
      Rvector3 control;

      // Now make the same call to the concatenator
      // Hack in some data
      FunctionInputData inData;
      PathFunctionContainer funData;
      Rvector6 cv;

      inData.SetPhaseNum(0);
      inData.SetTime(sc->GetEpoch());

      Rvector csaltState(84);

      sc->SetRealParameter("X", 6600.0);
      sc->SetRealParameter("Y", 6600.0);
      sc->SetRealParameter("Z", 6600.0);

      Rvector scState = sc->GetState(0);
      for (UnsignedInt i = 0; i < scState.GetSize(); ++i)
         csaltState[i] = scState[i];

//      Rvector sc2State = sc2->GetState(0);
//      for (UnsignedInt i = 42; i < sc2State.GetSize(); ++i)
//         csaltState[i] = sc2State[i];

      inData.SetStateVector(csaltState);
      inData.SetControlVector(cv);

      gpc->SetSource(dc, 0);
      funData.Initialize();

      gpc->SetParamData(&inData);
      gpc->SetFunctionData(&funData);

      std::cout << "Evaluating user function on the concatenator." << std::endl;
      // gpc->EvaluateUserFunction(&inData, &funData);
      gpc->EvaluateFunctions();
      Rvector catData = funData.GetDynData()->GetFunctionValues();

      std::cout << "\n\nData in the PathFunctionContainer after calling "
                << "EvaluateUserFunction():\n\n   " << catData.ToString();

//      gpc->EvaluateUserJacobian(&inData, &funData);
      gpc->EvaluateJacobians();
      Rmatrix catJData = funData.GetDynData()->GetStateJacobian();

      std::cout << "\n\nData in the PathFunctionContainer after calling "
                << "EvaluateUserJacobian():\n\n";
      for (UnsignedInt i = 0; i < catJData.GetNumRows(); ++i)
         std::cout << "   " << catJData.ToRowString(i, 12, 20) << "\n";

      std::cout << "\n\nCleaning up...";
      delete ode;
//      delete ode2;
      delete sc;
//      delete sc2;
      delete dc;
      delete gpc;

      std::cout << "Test run complete\n\n";
   }
   catch (BaseException &ex)
   {
      std::cout << "Exception caught in test run:\n\n"
                << ex.GetFullMessage()
                << "\nTest run failed\n\n";
   }

   return retval;
}
