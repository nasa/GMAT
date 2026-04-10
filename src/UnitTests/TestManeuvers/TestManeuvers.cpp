//------------------------------------------------------------------------------
//                               TestManeuvers
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
 * Unit test program for impulsive and (eventually) finite maneuvers. 
 */
//------------------------------------------------------------------------------


#include <iostream>

#include "ImpulsiveBurn.hpp"
#include "Spacecraft.hpp"
#include "ConsoleAppException.hpp"


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


void TestImpulsiveBurn(Spacecraft *sc = NULL, Real *instate = NULL)
{
    Real *state, start[6];
    Burn *burn = new ImpulsiveBurn("burn1");
    Integer id;
    
    if (sc) {
        id = burn->GetParameterID("SpacecraftName");
        burn->SetStringParameter(id, sc->GetName());
        state = sc->GetState();
        memcpy(start, state, 6*sizeof(Real));
    }
    else {
        if (instate)
            memcpy(start, instate, 6*sizeof(Real));
        else {
            start[0] = 7000.0;
            start[1] =    0.0;
            start[2] = 1000.0;
            start[3] =    0.0;
            start[4] =    5.15;
            start[5] =    5.05;
        }
        state = new Real[6];
    }
    
    
    // Test inertial reference frame
    memcpy(state, start, 6*sizeof(Real));

    id = burn->GetParameterID("CoordinateFrame");
    burn->SetStringParameter(id, "Inertial");

    std::cout << "Burn data:\n";

    Integer el = burn->GetParameterID("Element1");
    burn->SetRealParameter(el, 1.0);
    el = burn->GetParameterID("Element2");
    burn->SetRealParameter(el, 0.50);
    el = burn->GetParameterID("Element3");
    burn->SetRealParameter(el, 0.25);

    DumpObjectData(burn);
    
    std::cout << "\nPreburn state:\n   "
              << state[0] << "  " << state[1] << "  " << state[2] << "  " 
              << state[3] << "  " << state[4] << "  " << state[5] << "\n\n";

    burn->Fire(state);

    std::cout << "Postburn state:\n   "
              << state[0] << "  " << state[1] << "  " << state[2] << "  " 
              << state[3] << "  " << state[4] << "  " << state[5] << "\n\n";
        
    // Test VNB reference frame
    memcpy(state, start, 6*sizeof(Real));

    id = burn->GetParameterID("CoordinateFrame");
    burn->SetStringParameter(id, "VNB");
    el = burn->GetParameterID("Element2");
    burn->SetRealParameter(el, 0.0);
    el = burn->GetParameterID("Element3");
    burn->SetRealParameter(el, 0.0);

    std::cout << "Burn data:\n";

    DumpObjectData(burn);
    
    std::cout << "\nPreburn state:\n   "
              << state[0] << "  " << state[1] << "  " << state[2] << "  " 
              << state[3] << "  " << state[4] << "  " << state[5] << "\n\n";

    burn->Fire(state);

    std::cout << "Postburn state:\n   "
              << state[0] << "  " << state[1] << "  " << state[2] << "  " 
              << state[3] << "  " << state[4] << "  " << state[5] << "\n\n";
        
    delete burn;

    if (!sc)
        delete [] state;
}


int main(int argc, char **argv)
{
    try {
        std::cout << "************************************************\n"
                  << "*** Maneuver Unit Test Program\n"
                  << "************************************************\n\n";
        TestImpulsiveBurn();
        
        Spacecraft *sat = new Spacecraft("Fred");
        TestImpulsiveBurn(sat);
        delete sat;
    }
    catch (BaseException &ex) {
        std::cout << ex.GetMessage() << std::endl;
    }
    
//    std::cout << "Test complete -- Press Enter to exit\n";
//    std::cin.get();
}
