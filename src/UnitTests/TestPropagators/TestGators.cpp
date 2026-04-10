//------------------------------------------------------------------------------
//                               TestGators
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
// number NNG04CC06P
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2004/1/14
//
/**
 * Unit test program for propagators. 
 */
//------------------------------------------------------------------------------


#include "ForceModel.hpp"
#include "PointMassForce.hpp"
#include "DragForce.hpp"
#include "GravityField.hpp"
#include "SolarRadiationPressure.hpp"

#include "SolarSystem.hpp"
#include "SlpFile.hpp"
#include "ConsoleAppException.hpp"
#include "Spacecraft.hpp"

#include "PropSetup.hpp"
#include "RungeKutta89.hpp"
#include "DormandElMikkawyPrince68.hpp"


#include <iostream>


void DumpObjectData(GmatBase *obj)
{
    std::cout << "  Name:  " << obj->GetName() << "\n";
    Integer parms = obj->GetParameterCount(), i, type;
    for (i = 0; i < parms; ++i)
    {
        std::cout << "  " << obj->GetParameterText(i) << " = ";
        type = obj->GetParameterType(i);

        switch (type) {
            case Gmat::INTEGER_TYPE:
                std::cout << obj->GetIntegerParameter(i) << "\n";   
                break;
                
            case Gmat::REAL_TYPE:
                std::cout << obj->GetRealParameter(i) << "\n";   
                break;
                
            case Gmat::STRING_TYPE:
                std::cout << obj->GetStringParameter(i) << "\n";   
                break;
                
            case Gmat::BOOLEAN_TYPE:
                std::cout << obj->GetBooleanParameter(i) << "\n";   
                break;
                
            default:
                break;
        }
    }
    if (obj->GetType() == Gmat::BURN) {
        Integer id = obj->GetParameterID("CoordinateFrame");
        StringArray frames = obj->GetStringArrayParameter(id);
        std::cout << "  Available frames:\n";
        for (StringArray::iterator iter = frames.begin(); 
             iter != frames.end(); ++iter) {
            std::cout << "    " << *iter << "\n";
        }
    }                  
}


void TestProps(Real *instate = NULL)
{
    Real state[6], *satState;
    const Real *dv;
    Integer id;
    
    SolarSystem *solar = new SolarSystem("DefaultSolarSystem");
    /// @todo Need to fix the file path/name for platform dependencies
    SlpFile     *ephem = new SlpFile("mn2000.pc");
    solar->SetSourceFile(ephem);

    Spacecraft *sat = new Spacecraft("TestCraft");
    ForceModel *fm = new ForceModel("fm");
    fm->SetSolarSystem(solar);
    
    PhysicalModel *pmf;
    
    pmf = new PointMassForce;
    pmf->SetStringParameter("Body", "Earth");
    fm->AddForce(pmf);
    fm->AddSpacecraft(sat);
    
    PropSetup *ps = new PropSetup("prop");
    ps->SetForceModel(fm);
    
    Integrator **prop = new Integrator*[2];
    prop[1] = new RungeKutta89;
    prop[0] = new DormandElMikkawyPrince68;
    
    std::cout.precision(16);
    
    state[0] = 6000.0;
    state[1] = 1000.0;
    state[2] = 3000.0;
    state[3] = 1.0;
    state[4] = 6.7;
    state[5] = -3.0;
    
    satState = sat->GetState();
    
    std::cout << "Input State\n   Position: " 
              << satState[0] << ", " << satState[1] << ", " << satState[2] << "\n"
              << "   Velocity: "
              << satState[3] << ", " << satState[4] << ", " << satState[5]
              << std::endl;
              
    for (int i = 0; i < 2; ++i) {
       std::cout << "\nChecking " << prop[i]->GetTypeName() << std::endl;
       ps->SetPropagator(prop[i]);
       sat->SetState("Cartesian", state);
       std::cout << "Initial State\n   Position: " 
                 << satState[0] << ", " << satState[1] << ", " << satState[2] 
                 << "\n   Velocity: "
                 << satState[3] << ", " << satState[4] << ", " << satState[5]
                 << std::endl;
       ps->Initialize();
       prop[i]->Step(8640.0);
       fm->UpdateSpacecraft();
       std::cout << "Final State\n   Position: " 
                 << satState[0] << ", " << satState[1] << ", " << satState[2] 
                 << "\n   Velocity: "
                 << satState[3] << ", " << satState[4] << ", " << satState[5]
                 << std::endl;
    }

    delete ps;
    delete sat;
    delete solar;
}


int main(int argc, char **argv)
{
    try {
        std::cout << "************************************************\n"
                  << "*** Propagator Unit Test Program\n"
                  << "************************************************\n\n";
        TestProps();
    }
    catch (BaseException &ex) {
        std::cout << ex.GetMessage() << std::endl;
    }
}
