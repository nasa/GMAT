//------------------------------------------------------------------------------
//                           HohmannTransferDriver
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2020 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// You may not use this file except in compliance with the License.
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0.
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

#include "Driver.hpp"
#include "OcfObj1.hpp"
#include "OcfObj2.hpp"
#include "StateConversionUtil.hpp"
#include "CoordinateConverter.hpp"
#include "EphemSmoother.hpp"
#include "TransformUtil.hpp"

// SetPointPathAndProperties() method is where the Path, Point, and
// OptimalControlFunction classes will be instantiated.
void Driver::SetPointPathAndProperties()
{
   pathObject = new PathObj();
   pointObject = new PointObj();

   // Set CSALT and SNOPT-related parameters in this method
   // See CsaltDriver.hpp for some of the available options
   maxMeshRefinementCount = 25;
   feasibilityTolerances(0) = 1.0e-6;
   optimizationMode[0] = "Minimize";
}

void Driver::SetupPhases()
{
    //Set mesh properties
    RadauPhase* phase1 = new RadauPhase();
    // Initial guess modes: "LinearNoControl", "LinearUnityControl",
    // "LinearCoast", "OCHFile", "GuessArrays"
    std::string initialGuessMode = "LinearNoControl";
    Rvector meshIntervalFractions(11, -1.0, -0.8, -0.6, -0.4, -0.2, 0.0, 0.2, 0.4, 0.6, 0.8, 1.0);
    IntegerArray meshIntervalNumPoints = { 8, 8, 8, 8, 8, 8, 8, 8, 8, 8 };

    //Set time properties
    Real timeLowerBound = 0.0;
    Real timeUpperBound = 6.0 * GmatMathConstants::PI;
    Real initialGuessTime = 0.0;
    Real finalGuessTime = 2.0 * GmatMathConstants::PI;

    //Set state properties
    Integer numStateVars = 4;
    Rvector stateLowerBound(4, 0.0, 0.0, -10.0, -10.0);
    Rvector initialGuessState(4, 1.0, 0.0, 0.0, 1.0);
    Rvector finalGuessState(4, 1.1, 1.0 * GmatMathConstants::PI, 0.1, 1.0 / sqrt(1.1));
    Rvector stateUpperBound(4, 10.0, 12.0 * GmatMathConstants::PI, 10.0, 10.0);

    // Set static parameter properties
    Integer numStaticVars = 4;
    Real dvMagBD = 0.1;
    Rvector staticUpperBound(4, dvMagBD, dvMagBD, dvMagBD, dvMagBD);
    Rvector staticLowerBound(4, -dvMagBD, -dvMagBD, -dvMagBD, -dvMagBD);
    Rvector staticGuess(4, 0.00, 0.00, 0.00, 0.00);

    // Set mesh properties
    phase1->SetInitialGuessMode(initialGuessMode);
    phase1->SetNumStateVars(numStateVars);
    phase1->SetNumStaticVars(numStaticVars);
    phase1->SetMeshIntervalFractions(meshIntervalFractions);
    phase1->SetMeshIntervalNumPoints(meshIntervalNumPoints);
    phase1->SetStateLowerBound(stateLowerBound);
    phase1->SetStateUpperBound(stateUpperBound);
    phase1->SetStateInitialGuess(initialGuessState);
    phase1->SetStateFinalGuess(finalGuessState);
    phase1->SetTimeLowerBound(timeLowerBound);
    phase1->SetTimeUpperBound(timeUpperBound);
    phase1->SetTimeInitialGuess(initialGuessTime);
    phase1->SetTimeFinalGuess(finalGuessTime);
    
    phase1->SetNumStaticVars(numStaticVars);
    phase1->SetStaticLowerBound(staticLowerBound);
    phase1->SetStaticUpperBound(staticUpperBound);
    phase1->SetStaticGuess(staticGuess);
         
    phaseList.push_back(phase1);
}

void Driver::PostProcess(Trajectory* traj) {
    Phase* phase = traj->GetPhaseList()[0];
    Rvector time = phase->GetTimeVector();
    DecisionVector* dv = phase->GetDecisionVector();
    Rmatrix states = dv->GetStateArray();
    Rvector staticParams = dv->GetStaticVector();

    Integer n = states.GetNumRows();

    // Convert initial guess state to Keplerian orbital elements
    Real earthMu = 398600.4418;
    Real earthRadius = 6378.137;
    Real lstar = earthRadius;
    Real tstar = sqrt(pow(lstar, 3.0) / earthMu);

    Rvector t = time * tstar;
    Rvector r = states.GetColumn(0)*lstar;
    Rvector th = states.GetColumn(1);
    Rvector u = states.GetColumn(2)*lstar/tstar;
    Rvector v = states.GetColumn(3)*lstar/tstar;
    Rvector dv1(2, staticParams(0), staticParams(1));
    dv1 *= lstar / tstar;
    Rvector dv2(2, staticParams(2), staticParams(3));
    dv2 *= lstar / tstar;

    std::cout << "Minimum DV = " << dv1.Norm() + dv2.Norm() << " km/s" << std::endl;

    //------------------------------------------------------------------------------
    // GMAT Demo 1: Set up dynamical model 
    //------------------------------------------------------------------------------
    /**
     * Define solar system bodies, reference frames, and perturbing bodies
     */
    //------------------------------------------------------------------------------

    // Declare and instantiate SolarSystem object
    SolarSystem* solarSys = new SolarSystem("HohmannTransferSolarSys");

    // Declare and instantiate Earth CelestialBody object
    CelestialBody* earth; // object to store Earth body
    earth = solarSys->GetBody("Earth");
    // Declare and instantiate Sun CelestialBody object
    CelestialBody* sun;
    sun = solarSys->GetBody("Sun");
    // Declare and instantiate Luna CelestialBody object
    CelestialBody* luna;
    luna = solarSys->GetBody("Luna");

    // Declare and instantiate reference frames
    CoordinateSystem* earthMJ2000Eq;
    earthMJ2000Eq = CoordinateSystem::CreateLocalCoordinateSystem(
        "EarthMJ2000Eq", "MJ2000Eq", earth, NULL, NULL, earth->GetJ2000Body(), solarSys);
    CoordinateSystem* sunMJ2000Ec;
    sunMJ2000Ec = CoordinateSystem::CreateLocalCoordinateSystem(
        "SunMJ2000Ec", "MJ2000Ec", sun, NULL, NULL, sun->GetJ2000Body(), solarSys);
    CoordinateSystem* lunaMJ2000Ec;
    lunaMJ2000Ec = CoordinateSystem::CreateLocalCoordinateSystem(
        "LunaMJ2000Eq", "MJ2000Eq", luna, NULL, NULL, luna->GetJ2000Body(), solarSys);

    //------------------------------------------------------------------------------
    // GMAT Demo 2: Set up EphemerisSmoother object
    //------------------------------------------------------------------------------
    /**
     * Ephemeris smoother uses Spice ephemeris data to create a polynomial that approximates
     * the position and velocity of objects over a specified timespan. This speeds up computations
     * and ensures that velocity and acceleration values are the true derivatives of the position
     * and velocity values respectively.
     */
     //------------------------------------------------------------------------------
    // Create array of perturbing bodies to include in simulation; Earth is not included because it is the central body
    StringArray pertBodies;
    pertBodies.push_back("Sun");
    pertBodies.push_back("Luna");

    // Get NAIF IDs associated with each body
    std::vector<CelestialBody*> thirdBodyObjs;
    for (Integer ii = 0; ii < pertBodies.size(); ii++)
        thirdBodyObjs.push_back(solarSys->GetBody(pertBodies.at(ii)));

     // Ephemeris smoother class used to extract ephemerides more quickly and accurately
    EphemSmoother* ephemerisSmoother;

    // Define EphemSmoother settings
    Integer ephemSmoother_numRegionsPerRev = 359;
    Integer ephemSmoother_numRegions = 359;
    Real ephemSmoother_H = 1.0;
    std::string ephemSmoother_StepSizeType = "h";
    Real ephemSmoother_splineT0 = 25198.0;
    Real ephemSmoother_splineTf = 40000.0;

    // Instantiate and initialize EphemSmoother object using transfer and science orbit body list.
    ephemerisSmoother = new EphemSmoother(ephemSmoother_numRegionsPerRev, ephemSmoother_numRegions,
        ephemSmoother_H, ephemSmoother_StepSizeType, ephemSmoother_splineT0, ephemSmoother_splineTf);
    for (Integer ii = 0; ii < thirdBodyObjs.size(); ii++)
    {
        ephemerisSmoother->CreateSmoothedEphem(earth, thirdBodyObjs.at(ii),
            earthMJ2000Eq, ephemSmoother_splineT0, ephemSmoother_splineTf, ephemSmoother_numRegionsPerRev);
    }

    //------------------------------------------------------------------------------
    // GMAT Demo 3: Convert Between Reference Frames, State Representations, and Times
    //------------------------------------------------------------------------------
    /**
     * Provides examples of how to convert the between state representations only or
     * how to convert between state representations and reference frames. The timeConverter
     * is also demonstrated.
     */
     //------------------------------------------------------------------------------
    // Get key Earth parameters
    Real mu, flattening, eqRadius;
    mu = earth->GetGravitationalConstant();
    flattening = earth->GetFlattening();
    eqRadius = earth->GetEquatorialRadius();
     
    // Convert phase state to Keplerian state representation
    StateConversionUtil* stateConvUtil;
    Rvector6 stateKep;
    Rvector6 stateCart;
    Rmatrix statesCart(n, 6);
    Rmatrix statesKep(n, 6);
    for (Integer i = 0; i < n; ++i) {
        statesCart(i, 0) = stateCart(0) = r(i) * cos(th(i));
        statesCart(i, 1) = stateCart(1) = r(i) * sin(th(i));
        statesCart(i, 3) = stateCart(3) = u(i) * cos(th(i)) - v(i) * sin(th(i));
        statesCart(i, 4) = stateCart(4) = u(i) * sin(th(i)) + v(i) * cos(th(i));
        stateKep = stateConvUtil->Convert(stateCart, "Cartesian", "Keplerian", mu, flattening, eqRadius);
        for (Integer j = 0; j < 6; ++j)
            statesKep(i, j) = stateKep(j);
    }

    FILE* fid;
    fid = fopen("statesKep.dat", "w");
    for (Integer i = 0; i < n; ++i)
        fprintf(fid, "% .16f % .16f % .16f % .16f % .16f % .16f % .16f\n", 
            t(i), 
            statesKep(i, 0), 
            statesKep(i, 1), 
            statesKep(i, 2), 
            statesKep(i, 3), 
            statesKep(i, 4), 
            statesKep(i, 5));
    fclose(fid);

    // Convert from UTC to A1 time system
    // Declare and instantiate TimeSystemConverter object
    TimeSystemConverter* timeConverter;
    timeConverter = timeConverter->Instance();

    Integer a1TimeID = timeConverter->GetTimeTypeID("A1Mjd");
    Integer utcTimeID = timeConverter->GetTimeTypeID("UtcMjd");
    Real utcTime, a1Time;
    utcTime = 32198.0; // should be within T0 anf Tf bounds set for the smoothed ephemeris spline

    // Convert phase state to Keplerian coordinates in the Sun-centered ecliptic frame
    Rmatrix CartStates_sunMJ2000Ec(n, 6);
    Rvector6 CartState_sunMJ2000Ec;
    for (Integer i = 0; i < n; ++i) {
        a1Time = timeConverter->Convert(utcTime + 3.1688087814029E-08 * t(i), utcTimeID, a1TimeID);
        for (Integer j = 0; j < 6; ++j)
            stateCart(j) = statesCart(i, j);
        CartState_sunMJ2000Ec = TransformUtil::TransformOrbitalState(stateCart,
            a1Time, earthMJ2000Eq, "Cartesian", sunMJ2000Ec, "Cartesian", "Spline", ephemerisSmoother);
        for (Integer j = 0; j < 6; ++j)
            CartStates_sunMJ2000Ec(i, j) = CartState_sunMJ2000Ec(j);
    }

    fid = fopen("CartStates_sunMJ2000Ec.dat", "w");
    for (Integer i = 0; i < n; ++i)
        fprintf(fid, "% .16f % .16f % .16f % .16f % .16f % .16f % .16f\n", 
            t(i), 
            CartStates_sunMJ2000Ec(i, 0),
            CartStates_sunMJ2000Ec(i, 1),
            CartStates_sunMJ2000Ec(i, 2),
            CartStates_sunMJ2000Ec(i, 3),
            CartStates_sunMJ2000Ec(i, 4),
            CartStates_sunMJ2000Ec(i, 5));
    fclose(fid);

    // Get distance from Moon's center
    Rvector6 KepState_lunaMJ2000Ec;
    Rvector MoonPosition(3);
    Rvector MoonDistance(n);
    for (Integer i = 0; i < n; ++i) {
        a1Time = timeConverter->Convert(utcTime + 3.1688087814029E-08 * t(i), utcTimeID, a1TimeID);
        for (Integer j = 0; j < 6; ++j)
            stateCart(j) = statesCart(i, j);
        KepState_lunaMJ2000Ec = TransformUtil::TransformOrbitalState(stateCart,
            a1Time, earthMJ2000Eq, "Cartesian", lunaMJ2000Ec, "Cartesian", "Spline", ephemerisSmoother);
        for (Integer j = 0; j < 3; ++j)
            MoonPosition(j) = KepState_lunaMJ2000Ec(j);
        MoonDistance(i) = MoonPosition.Norm();
    }
    std::cout << "Initial distance from the Moon = " << MoonDistance(0) << " km" << std::endl;
    std::cout << "Final distance from the Moon = " << MoonDistance(n - 1) << " km" << std::endl;

    if (solarSys)
        delete solarSys;
    if (ephemerisSmoother)
        delete ephemerisSmoother;
}

