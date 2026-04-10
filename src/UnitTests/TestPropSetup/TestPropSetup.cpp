//------------------------------------------------------------------------------
//                                  TestPropSetup
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
// Created: 2003/10/16
//
/**
 * Test driver for PropSetup.
 */
//------------------------------------------------------------------------------
#include <iostream>
#include "gmatdefs.hpp"
#include "ForceModel.hpp"
#include "PropSetup.hpp"
#include "RungeKutta89.hpp"

using namespace std;

int main(int argc, char *argv[])
{
  
   cout << "============================== test PropSetup" << endl;
   RungeKutta89 prop1 = RungeKutta89("Prop1");
   ForceModel fm1 = ForceModel("FM1");
   
   cout << "---------- test ps1 = PropSetup('PropSetup1', &prop1, &fm1)" << endl;
   PropSetup ps1 = PropSetup("PropSetup1", &prop1, &fm1);   
   cout << "Instance Name = " << ps1.GetName() << " Type = " <<
      ps1.GetType() << " Type Name = " << ps1.GetTypeName() << endl;
   cout << "IsInitialized() should be true" << endl;
   cout << ps1.IsInitialized() << endl;
   
   cout << "---------- test ps2 = PropSetup()" << endl;
   PropSetup ps2 = PropSetup("PropSetup2");
   cout << "Instance Name = " << ps2.GetName() << " Type = " <<
      ps2.GetType() << " Type Name = " << ps2.GetTypeName() << endl;
   cout << "IsInitialized() should be false" << endl;
   cout << ps2.IsInitialized() << endl;

   cout << "---------- test PropSetup(ps1) copy constructor" << endl;
   PropSetup ps3 = PropSetup(ps1);
   cout << "Instance Name = " << ps3.GetName() << " Type = " <<
      ps3.GetType() << " Type Name = " << ps3.GetTypeName() << endl;
   cout << "IsInitialized() should be true" << endl;
   cout << ps3.IsInitialized() << endl;
   
   cout << "---------- test ps4 = ps2 assignment operator" << endl;
   PropSetup ps4("PropSetup4");
   ps4 = ps2;
   cout << "Instance Name = " << ps4.GetName() << " Type = " <<
      ps4.GetType() << " Type Name = " << ps4.GetTypeName() << endl;
   cout << "IsInitialized() should be false" << endl;
   cout << ps4.IsInitialized() << endl;

   RungeKutta89 prop2(prop1);
   ForceModel fm2(fm1);
   
   cout << "---------- test SetPropagator(&prop2)" << endl;
   ps4.SetPropagator(&prop2);
   cout << "IsInitialized() should be false" << endl;
   cout << ps4.IsInitialized() << endl;
   
   cout << "---------- test SetForceModel(&fm2)" << endl;
   ps4.SetForceModel(&fm2);
   cout << "IsInitialized() should be true" << endl;
   cout << ps4.IsInitialized() << endl;
   
   cout << "---------- test GetPropagator()  Name should have CopyOfProp1 " << endl;
   Propagator *tempProp = ps4.GetPropagator();
   cout << "Instance Name = " << tempProp->GetName() << " Type = " <<
      tempProp->GetType() << " Type Name = " << tempProp->GetTypeName() << endl;
   
   cout << "---------- test GetForceModel() Name should have CopyOfFM1" << endl;
   Propagator *tempFm = ps4.GetPropagator();
   cout << "Instance Name = " << tempFm->GetName() << " Type = " <<
      tempFm->GetType() << " Type Name = " << tempFm->GetTypeName() << endl;
   
   cout << "---------- test virtual GetStringParameter(PropagatorName)" << endl;
   Integer propId = ps4.GetParameterID("PropagatorName");
   cout << "ID = " << propId << " Val = " << ps4.GetStringParameter(propId) << endl;
   
   cout << "---------- test virtual GetStringParameter(ForceModelName)" << endl;
   Integer fmId = ps4.GetParameterID("ForceModelName");
   cout << "ID = " << fmId << " Val = " << ps4.GetStringParameter(fmId) << endl;
   
   cout << "---------- test virtual SetStringParameter(propId, 'NewProp')" << endl <<
      "should return true" << endl;
   cout << ps4.SetStringParameter(propId, "NewProp") << endl;
   cout << "---------- test virtual GetStringParameter(propId) should return 'NewProp'" << endl;
   cout << "ID = " << propId << " Val = " << ps4.GetStringParameter(propId) << endl;

   propId = -1;
   cout << "---------- test virtual SetStringParameter(-1, 'NewProp')" << endl <<
      "should return false" << endl;
   cout << ps4.SetStringParameter(propId, "NewProp") << endl;
   cout << "---------- test virtual GetStringParameter(propId) should return empty string" << endl;
   cout << "ID = " << propId << " Val = " << ps4.GetStringParameter(propId) << endl;
   
   cout << "---------- test GetParameterCount()" << endl;
   int paramCount = ps4.GetParameterCount();
   cout << paramCount << endl;
   
   cout << "---------- test virtual GetParameterList()" << endl;
   const std::string *paramList;
   paramList = ps4.GetParameterList();
   for (int i=0; i<paramCount; i++)
      cout << paramList[i] << endl;
   
   cout << "---------- ps4.SetPropagator(rkv89) - set to rkv89" << endl;
   RungeKutta89 *rkv89 = new RungeKutta89("RKV89");
   ps4.SetPropagator(rkv89);
   cout << ps4.GetPropagator()->GetName() << endl;
   
   cout << "---------- ps4.GetStringParameter(propId)" << endl;
   propId = ps4.GetParameterID("Type");
   cout << "ID = " << propId << " PropType = " << ps4.GetStringParameter(propId) << endl;



   //-----------------------------------
   cout << endl;
   cout << "Hit enter to end" << endl;
   cin.get();
   //-----------------------------------
}
