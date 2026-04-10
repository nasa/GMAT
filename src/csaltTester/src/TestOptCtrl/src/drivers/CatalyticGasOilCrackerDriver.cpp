//------------------------------------------------------------------------------
//                           CatalyticGasOilCrackerDriver
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

#include "CatalyticGasOilCrackerDriver.hpp"
#include "CatalyticGasOilCrackerPathObject.hpp"
#include "CatalyticGasOilCrackerPointObject.hpp"

static const Real INF = std::numeric_limits<Real>::infinity();

CatalyticGasOilCrackerDriver::CatalyticGasOilCrackerDriver() :
   CsaltTestDriver("CatalyticGasOilCracker")
{
}

CatalyticGasOilCrackerDriver::~CatalyticGasOilCrackerDriver()
{
}

void CatalyticGasOilCrackerDriver::SetPointPathAndProperties()
{
   pathObject = new CatalyticGasOilCrackerPathObject();
   pointObject = new CatalyticGasOilCrackerPointObject();

   // Also set things like the bounds, mesh refinement count, and so forth here
   maxMeshRefinementCount = 2;
}

void CatalyticGasOilCrackerDriver::SetupPhases()
{
   try
   {
      // Cost bounds
      Real costLowerBound = 0.0;
      Real costUpperBound = 1e4;

      std::string initialGuessMode = "LinearNoControl";
      Rvector meshIntervalFractions(2, -1.0, 1.0);
      IntegerArray meshIntervalNumPoints;
      meshIntervalNumPoints.push_back(4);

      // Set state properties
      Integer numStateVars = 2;
      Rvector stateLowerBound(2, 0.0, 0.0);
      Rvector stateUpperBound(2, 2.0, 2.0);

      // Set time properties
      Real timeLowerBound = 0.0;
      Real timeUpperBound = 0.95;



      // Set control properties
      Integer numControlVars = 0;
      Integer numStaticVars = 3;

      Rvector tmeas(21, 0.0, 0.025, 0.05, 0.075, 0.1, 0.125, 0.15, 0.175, 0.2, 0.225, 0.25,
         0.3, 0.35, 0.4, 0.45, 0.5, 0.55, 0.65, 0.75, 0.85, 0.95);

      Rvector y1meas(21, 1.0, 0.8105, 0.6208, 0.5258, 0.4345, 0.3903, 0.3342,
         0.3034, 0.2735, 0.2405, 0.2283, 0.2071, 0.1669, 0.153, 0.1339,
         0.1265, 0.12, 0.099, 0.087, 0.077, 0.069);
      // Measured values of y2 
      Rvector y2meas(21, 0.0, 0.2, 0.2886, 0.301, 0.3215, 0.3123, 0.2716, 0.2551,
         0.2258, 0.1959, 0.1789, 0.1457, 0.1198, 0.0909, 0.0719, 0.0561,
         0.046, 0.028, 0.019, 0.014, 0.01);

      Integer numPhases = 20;
      Real costFunc = 0.0;
      Phase *phase1[20];


      Rvector staticUpperBound(3, 20.0, 20.0, 20.0);
      Rvector staticLowerBound(3, 0.0, 0.0, 0.0);
      Rvector staticGuess(3, 0.0, 0.0, 0.0);


      std::string rkMode = "RungeKutta8";
      
      for (Integer idx = 0; idx < numPhases; idx++)
      {
         //phase1[idx] = new RadauPhase();

         ImplicitRKPhase *rkphase = new ImplicitRKPhase();
         rkphase->SetTranscription(rkMode);
         phase1[idx] = rkphase;

         Real initialGuessTime = tmeas(idx);
         Real finalGuessTime = tmeas(idx+1);

         Rvector initialGuessState(2, y1meas(idx), y2meas(idx));
         Rvector finalGuessState(2, y1meas(idx+1), y2meas(idx+1));
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
         phase1[idx]->SetStateInitialGuess(initialGuessState);
         phase1[idx]->SetStateFinalGuess(finalGuessState);
         phase1[idx]->SetTimeLowerBound(timeLowerBound);
         phase1[idx]->SetTimeUpperBound(timeUpperBound);
         phase1[idx]->SetTimeInitialGuess(initialGuessTime);
         phase1[idx]->SetTimeFinalGuess(finalGuessTime);
         

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

