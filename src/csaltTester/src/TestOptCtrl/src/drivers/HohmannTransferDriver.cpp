//------------------------------------------------------------------------------
//                           HohmannTransferDriver
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under Purchase
// Order NNG16LD52P
//
// Author: Joshua Raymond, Thinking Systems, Inc.
// Created: Mar 20, 2018
//------------------------------------------------------------------------------

#include "HohmannTransferDriver.hpp"
#include "HohmannTransferPathObject.hpp"
#include "HohmannTransferPointObject.hpp"

static const Real INF = std::numeric_limits<Real>::infinity();

HohmannTransferDriver::HohmannTransferDriver() :
   CsaltTestDriver("HohmannTransfer")
{
}

HohmannTransferDriver::~HohmannTransferDriver()
{
}

void HohmannTransferDriver::SetPointPathAndProperties()
{
   pathObject = new HohmannTransferPathObject();
   pointObject = new HohmannTransferPointObject();

   // Also set things like the bounds, mesh refinement count, and so forth here
   maxMeshRefinementCount = 5;
}

void HohmannTransferDriver::SetupPhases()
{
   try
   {
      // Cost bounds
      Real costLowerBound = 0.0;
      Real costUpperBound = 1.0;

      std::string initialGuessMode = "LinearNoControl";
      Rvector meshIntervalFractions(11, -1.0,   -0.8, -0.6, -0.4, -0.2,  
         0.0, 0.2, 0.4, 0.6, 0.8, 1.0);
      IntegerArray meshIntervalNumPoints;
      meshIntervalNumPoints.push_back(8);
      meshIntervalNumPoints.push_back(8);
      meshIntervalNumPoints.push_back(8);
      meshIntervalNumPoints.push_back(8);
      meshIntervalNumPoints.push_back(8);
                                      
      meshIntervalNumPoints.push_back(8);
      meshIntervalNumPoints.push_back(8);
      meshIntervalNumPoints.push_back(8);
      meshIntervalNumPoints.push_back(8);
      meshIntervalNumPoints.push_back(8);
                                      



      // Set control properties
      Integer numControlVars = 0;
      Integer numStaticVars = 4;

      //Set time properties
      Real timeLowerBound = 0.0;
      Real timeUpperBound = 6.0 * GmatMathConstants::PI;
      Real initialGuessTime = 0.0;
      Real finalGuessTime = 2.0 * GmatMathConstants::PI;

      //Set state properties
      Integer numStateVars = 4;
      Rvector stateLowerBound(4, 0.0, 0.0 * 2.0 * GmatMathConstants::PI, -10.0, -10.0);
      Rvector stateUpperBound(4, 10.0, 6.0 * 2.0 * GmatMathConstants::PI, 10.0, 10.0);

      Rvector initialGuessState(4, 1.0, 0.0, 0.0, 1.0);
      Rvector finalGuessState(4, 1.1, 1.0 * GmatMathConstants::PI, 0.1, 1.0/sqrt(1.1));


      Integer numPhases = 1;
      Phase *phase1[1];

      Real dvMagBD = 0.1;
      Rvector staticUpperBound(4, dvMagBD, dvMagBD, dvMagBD, dvMagBD);
      Rvector staticLowerBound(4, -dvMagBD, -dvMagBD, -dvMagBD, -dvMagBD);
      Rvector staticGuess(4, 0.00, 0.00, 0.00, 0.00);


      
         //phase1[idx] = new RadauPhase();

      RadauPhase *rdphase = new RadauPhase();
      for (Integer idx = 0; idx < 1; idx++)
      { 
         phase1[idx] = rdphase;

         // PHASE 1
         // Set mesh properties
         phase1[idx]->SetInitialGuessMode(initialGuessMode);
         phase1[idx]->SetNumStateVars(numStateVars);
         phase1[idx]->SetNumStaticVars(numStaticVars);
         phase1[idx]->SetNumControlVars(numControlVars);
         phase1[idx]->SetMeshIntervalFractions(meshIntervalFractions);
         phase1[idx]->SetMeshIntervalNumPoints(meshIntervalNumPoints);
         phase1[idx]->SetStateLowerBound(stateLowerBound);
         phase1[idx]->SetStateUpperBound(stateUpperBound);
         phase1[idx]->SetTimeLowerBound(timeLowerBound);
         phase1[idx]->SetTimeUpperBound(timeUpperBound);
         phase1[idx]->SetTimeInitialGuess(initialGuessTime);
         phase1[idx]->SetTimeFinalGuess(finalGuessTime);

         phase1[idx]->SetStateInitialGuess(initialGuessState);
         phase1[idx]->SetStateFinalGuess(finalGuessState);
         phase1[idx]->SetNumStaticVars(numStaticVars);
         phase1[idx]->SetStaticLowerBound(staticLowerBound);
         phase1[idx]->SetStaticUpperBound(staticUpperBound);
         phase1[idx]->SetStaticGuess(staticGuess);
         
         phaseList.push_back(phase1[idx]);
      }
      
   }
   catch (LowThrustException &ex)
   {
      MessageInterface::ShowMessage("%s\n", ex.GetDetails().c_str());
   }
}

