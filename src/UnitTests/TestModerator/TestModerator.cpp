//------------------------------------------------------------------------------
//                                  TestModerator
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
// Created: 2003/10/20
//
/**
 * Test driver for GuiInterpreter and Moderator.
 * Moderator is tested through GuiInterpreter.
 */
//------------------------------------------------------------------------------
#include <iostream>
#include "gmatdefs.hpp"
#include "Moderator.hpp"
#include "GuiInterpreter.hpp"
#include "Spacecraft.hpp"
#include "PropSetup.hpp"
#include "Propagator.hpp"
#include "Subscriber.hpp"
#include "Command.hpp"
#include "Burn.hpp"
#include "AtmosphereModel.hpp"
#include "Function.hpp"
#include "TestOutput.hpp"

using namespace std;

//------------------------------------------------------------------------------
//int RunTest(TestOutput &out)
//------------------------------------------------------------------------------
int RunTest(TestOutput &out)
{
   
   out.Put("============================== test Moderator");
   out.Put("---------- test Moderator::Instance()");
   Moderator *mod = Moderator::Instance();

   out.Put("---------- test Moderator::Initialize()");
   mod->Initialize();
   
   out.Put("---------- test Moderator::GetGuiInterpreter()");
   GuiInterpreter *guiInterp = mod->GetGuiInterpreter();
   if (!guiInterp)
   {
      out.Put("GuiInterpreter is NULL");
      return 0;
   }
   
   out.Put("---------- test Moderator::GetScriptInterpreter()");
   ScriptInterpreter *scriptInterp = mod->GetScriptInterpreter();
   if (!scriptInterp)
   {
      out.Put("ScriptInterpreter is NULL");
      return 0;
   }
   
   out.Put("---------- test mod->GetObjectTypeString(Gmat::SPACECRAFT)");
   out.Put("should return 'Spacecraft'");
   out.Put(mod->GetObjectTypeString(Gmat::SPACECRAFT));

   out.Put("---------- test mod->GetObjectTypeString(Gmat::PROP_SETUP)");
   out.Put("should return 'PropSetup'");
   out.Put(mod->GetObjectTypeString(Gmat::PROP_SETUP));

   out.Put("---------- test mod->GetObjectTypeString(Gmat::UNKNOWN_OBJECT)");
   out.Put("should return 'UnknownObject'");
   out.Put(mod->GetObjectTypeString(Gmat::UNKNOWN_OBJECT));

   out.Put("---------- test guiInterp->IsInitialized()");
   out.Put("should return false: ", guiInterp->IsInitialized());
   
   out.Put("---------- test guiInterp->Initialize()");
   guiInterp->Initialize();
   out.Put("IsInitialized() should return true: ", guiInterp->IsInitialized());

   // FactoryManager
   out.Put("\n============================== Test FactoryManager - FactoryItems");
   out.Put("");
   out.Put("---------- test guiInterp->GetListOfFactoryItems(Gmat::PROP_SETUP)");
   StringArray creatablePropSetup = guiInterp->GetListOfFactoryItems(Gmat::PROP_SETUP);
   out.Put(creatablePropSetup[0]);

   out.Put("");
   out.Put("---------- test guiInterp->GetListOfFactoryItems(Gmat::BURN)");
   StringArray creatableBurns = guiInterp->GetListOfFactoryItems(Gmat::BURN);
   out.Put(creatableBurns[0]);

   out.Put("");
   out.Put("---------- test guiInterp->GetListOfFactoryItems(Gmat::ATMOSPHERE)");
   StringArray creatableAms = guiInterp->GetListOfFactoryItems(Gmat::ATMOSPHERE);
   for (unsigned int i=0; i<creatableAms.size(); i++)
      out.Put(creatableAms[i]);

   out.Put("");
   out.Put("---------- test guiInterp->GetListOfFactoryItems(Gmat::FUNCTION)");
   StringArray creatableFns = guiInterp->GetListOfFactoryItems(Gmat::FUNCTION);
   for (unsigned int i=0; i<creatableFns.size(); i++)
      out.Put(creatableFns[i]);

   out.Put("");
   out.Put("---------- test guiInterp->GetListOfFactoryItems(Gmat::CALCULATED_POINT)");
   StringArray creatableCpss = guiInterp->GetListOfFactoryItems(Gmat::CALCULATED_POINT);
   for (unsigned int i=0; i<creatableCpss.size(); i++)
      out.Put(creatableCpss[i]);

   out.Put("\n============================== Test FactoryManager - Create Objects\n");
   // Create Spacecraft
   out.Put("---------- test guiInterp->CreateSpacecraft('Spacecraft', 'sc1')");
   
   Spacecraft *sc1 = guiInterp->CreateSpacecraft("Spacecraft", "sc1");
   if (!sc1)
      return 0;
   
   out.Put("spacecraft name = ", sc1->GetName());
   
   // Create Hardware
   out.Put("---------- test guiInterp->CreateHardware('FuelTank', 'fueltank1')");
   Hardware *hw1 = guiInterp->CreateHardware("FuelTank", "fueltank1");
   if (!hw1)
      return 0;
   
   out.Put("hardware type = ", hw1->GetTypeName());
   out.Put("         name = ", hw1->GetName());
   out.Put("           test guiInterp->GetHardware('fueltank1')");
   hw1 = guiInterp->GetHardware("fueltank1");
   if (!hw1)
      return 0;
   
   out.Put("hardware type = ", hw1->GetTypeName());
   out.Put("         name = ", hw1->GetName());
   
   // Create Propagator
   out.Put("---------- test guiInterp->CreatePropagator('RungeKutta89', 'rkv1')");
   Propagator *prop1 = guiInterp->CreatePropagator("RungeKutta89", "rkv1");
   if (!prop1)
      return 0;
   
   out.Put("propagator name = ", prop1->GetName());

   // Create DefaultPropSetup
   PropSetup *propSetup1 = guiInterp->CreateDefaultPropSetup("MyPropSetup");
   if (!propSetup1)
      return 0;
   
   out.Put("PropSetup name = ", propSetup1->GetName());
   
   // Create Subscriber
   Subscriber *rpt1 = guiInterp->CreateSubscriber("ReportFile", "rpt1");
   if (!rpt1)
      return 0;
   
   // Create GmatCommand
   GmatCommand *cmd = guiInterp->CreateCommand("Propagate", "propagate1");
   if (!cmd)
      return 0;
   
   // Create Burn
   Burn *burn = guiInterp->CreateBurn("ImpulsiveBurn", "impBurn1");
   if (!burn)
      return 0;
   
   // Create AtmosphereModel (Exponential)
   out.Put("---------- test guiInterp->CreateAtmosphereModel('Exponential', 'expmodel')");
   AtmosphereModel *expModel =
      guiInterp->CreateAtmosphereModel("Exponential", "expmodel");
   if (!expModel)
      return 0;
   
   out.Put(expModel->GetName(), " Created.");
   
   // Create AtmosphereModel (MSISE90)
   out.Put("---------- test guiInterp->CreateAtmosphereModel('MSISE90', 'msmodel')");
   AtmosphereModel *msModel =
      guiInterp->CreateAtmosphereModel("MSISE90", "msmodel");
   if (!msModel)
      return 0;
   
   out.Put(msModel->GetName(), " Created.");
   
   // Create AtmosphereModel (JacchiaRoberts)
   out.Put("---------- test guiInterp->CreateAtmosphereModel('JacchiaRoberts', 'jrmodel')");
   AtmosphereModel *jrModel =
      guiInterp->CreateAtmosphereModel("JacchiaRoberts", "jrmodel");
   if (!jrModel)
      return 0;
   
   out.Put(jrModel->GetName(), " Created.");
   
   // Create Function (MatlabFunction)
   out.Put("---------- test guiInterp->CreateFunction('MatlabFunction', 'function1')");
   Function *fn1 =
      guiInterp->CreateFunction("MatlabFunction", "function1");
   if (!fn1)
      return 0;
   
   out.Put(fn1->GetName(), " Created.");
   
   // Create Function (MatlabFunction)
   out.Put("---------- test guiInterp->CreateFunction('MatlabFunction', 'function2')");
   Function *fn2 =
      guiInterp->CreateFunction("MatlabFunction", "function2");
   if (!fn2)
      return 0;
   
   out.Put(fn2->GetName(), " Created.");
   
   // Create CalculatedPoint (Barycenter)
   out.Put("---------- test guiInterp->CreateCalculatedPoint('Barycenter', 'baryCenter1')");
   CalculatedPoint *bc1 =
      guiInterp->CreateCalculatedPoint("Barycenter", "baryCenter1");
   if (!bc1)
      return 0;
   
   out.Put(bc1->GetName(), " Created.");

   // ConfigManager
   out.Put("\n============================== Test ConfigManager\n");
   out.Put("---------- test guiInterp->GetListOfConfiguredItems(Gmat::BURN)");
   StringArray configuredBurn = guiInterp->GetListOfConfiguredItems(Gmat::BURN);
   out.Put(configuredBurn[0]);

   out.Put("");
   out.Put("---------- test guiInterp->GetListOfConfiguredItems(Gmat::ATMOSPHERE)");
   StringArray configuredAtmosModel = guiInterp->GetListOfConfiguredItems(Gmat::ATMOSPHERE);
   for (unsigned int i=0; i<configuredAtmosModel.size(); i++)
      out.Put(configuredAtmosModel[i]);
      
   out.Put("");
   out.Put("---------- test guiInterp->GetListOfConfiguredItems(Gmat::FUNCTION)");
   StringArray configuredFunction = guiInterp->GetListOfConfiguredItems(Gmat::FUNCTION);
   for (unsigned int i=0; i<configuredFunction.size(); i++)
      out.Put(configuredFunction[i]);
      
   out.Put("");
   out.Put("---------- test guiInterp->GetListOfConfiguredItems(Gmat::CALCULATED_POINT)");
   StringArray configuredCp = guiInterp->GetListOfConfiguredItems(Gmat::CALCULATED_POINT);
   for (unsigned int i=0; i<configuredCp.size(); i++)
      out.Put(configuredCp[i]);

   out.Put("");
   out.Put("---------- test Moderator::Finalize()");
   mod->Finalize();

   return 1;
}


//------------------------------------------------------------------------------
// int main(int argc, char *argv[])
//------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
   //loj: 7/13/05 changed output name from TestModerator.out
   TestOutput out("..\\..\\Test\\TestModerator\\TestModeratorOut.txt");
   out.SetPrecision(12);
   
   try
   {
      int status = RunTest(out);
      if (status == 0)
         out.Put("\nerror occurred during unit testing of the GuiInterpreter/Moderator!!");
      else
         out.Put("\nSuccessfully ran unit testing of the GuiInterpreter/Moderator!!");
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
