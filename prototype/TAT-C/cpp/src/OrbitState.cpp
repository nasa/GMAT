//------------------------------------------------------------------------------
//                           OrbitState
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
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
// Author: Wendy Shoan, NASA/GSFC
// Created: 2016.05.04
//
/**
 * Implementation of the OrbitState class
 */
//------------------------------------------------------------------------------

#include <cmath>            // for INFINITY
#include "gmatdefs.hpp"
#include "OrbitState.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "Rmatrix33.hpp"
#include "StateConversionUtil.hpp"
#include "TATCException.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// OrbitState()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 *
 */
//------------------------------------------------------------------------------
OrbitState::OrbitState() :
   mu         (3.986004415e+5)
{
   currentState.Set(7100.0, 0.0, 2000.0,
                       0.0, 7.4,    1.0);
}

//------------------------------------------------------------------------------
// OrbitState(const OrbitState &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param copy  the OrbitState object to copy
 *
 */
//------------------------------------------------------------------------------
OrbitState::OrbitState(const OrbitState &copy) :
   currentState (copy.currentState),
   mu           (copy.mu)
{
}

//------------------------------------------------------------------------------
// OrbitState& operator=(const OrbitState &copy)
//------------------------------------------------------------------------------
/**
 * The operator= for OrbitState.
 * 
 * @param copy  the OrbitState object to copy
 *
 */
//------------------------------------------------------------------------------
OrbitState& OrbitState::operator=(const OrbitState &copy)
{
   if (&copy == this)
      return *this;
   
   currentState = copy.currentState;
   mu           = copy.mu;
   
   return *this;
}

//------------------------------------------------------------------------------
// ~OrbitState()
//------------------------------------------------------------------------------
/**
 * Destructor.
 *
 */
//------------------------------------------------------------------------------
OrbitState::~OrbitState()
{
}

//------------------------------------------------------------------------------
// void SetKeplerianState(Real SMA, Real ECC,
//                        Real INC, Real RAAN,
//                        Real AOP, Real TA);
//------------------------------------------------------------------------------
/**
 * Sets the keplerian state, element by element.
 * 
 * @param SMA  semimajor axis
 * @param ECC  eccentricity
 * @param INC  inclination
 * @param RAAN right ascension of the ascending node
 * @param AOP  argument of periapsis
 * @param TA   true anomaly
 *
 */
//------------------------------------------------------------------------------
void OrbitState::SetKeplerianState(Real SMA, Real ECC,
                                   Real INC, Real RAAN,
                                   Real AOP, Real TA)
{
   // Sets the Keplerian state
   // Angles are in radians, SMA unit should be consistent with gravParam
   currentState = ConvertKeplerianToCartesian(SMA,ECC,INC,RAAN,AOP,TA);
}

//------------------------------------------------------------------------------
// void SetKeplerianVectorState(const Rvector6 &kepl)
//------------------------------------------------------------------------------
/**
 * Sets the keplerian state, as a 6-element vector.
 * 
 * @param kepl  keplerian state
 *
 */
//------------------------------------------------------------------------------
void OrbitState::SetKeplerianVectorState(const Rvector6 &kepl)
{
   // Sets Keplerian state given states in an array
   // Angles are in radians, SMA unit should be consistent with gravParam
   currentState = ConvertKeplerianToCartesian(kepl(0),kepl(1),kepl(2),
                                              kepl(3),kepl(4),kepl(5));
}

//------------------------------------------------------------------------------
// void SetCartesianState(const Rvector6 &cart)
//------------------------------------------------------------------------------
/**
 * Sets the cartesian state, as a 6-element vector.
 * 
 * @param cart  cartesian state (units should be consistent with gravParam)
 *
 */
//------------------------------------------------------------------------------
void OrbitState::SetCartesianState(const Rvector6 &cart)
{
   // Sets the cartesian state
   // Units should be consistent with gravParam
   currentState = cart;
}

//------------------------------------------------------------------------------
// void SetGravityParameter(Real toGrav)
//------------------------------------------------------------------------------
/**
 * Sets the gravity parameter.
 * 
 * @param toGrav  gravity parameter
 *
 */
//------------------------------------------------------------------------------
void OrbitState::SetGravityParameter(Real toGrav)
{
   mu = toGrav;
}

//------------------------------------------------------------------------------
// Rvector6 GetKeplerianState()
//------------------------------------------------------------------------------
/**
 * Returns the keplerian state as a 6-element vector
 * 
 * @return  keplerian state
 *
 */
//------------------------------------------------------------------------------
Rvector6 OrbitState::GetKeplerianState()
{
   // Returns the Keplerian state
   // Angles are in radians, SMA unit is consistent with g
   return ConvertCartesianToKeplerian(currentState);
}

//------------------------------------------------------------------------------
// Rvector6 GetCartesianState()
//------------------------------------------------------------------------------
/**
 * Returns the cartesian state as a 6-element vector
 * 
 * @return  cartesian state
 *
 */
//------------------------------------------------------------------------------
Rvector6 OrbitState::GetCartesianState()
{
   // Returns the Cartesian state
   // Units are consistent with g
   return currentState;
}



//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Rvector6 ConvertKeplerianToCartesian(Real a,  Real e,
//                                      Real i,  Real Om,
//                                      Real om, Real nu)
//------------------------------------------------------------------------------
/**
 * Converts the keplerian state to a cartesian state.
 * 
 * @param a    semimajor axis
 * @param e    eccentricity
 * @param i    inclination
 * @param Om   right ascension of the ascending node
 * @param om   argument of periapsis
 * @param nu   true anomaly
 * 
 * @return the cartesian state as a 6-element vector
 *
 */
//------------------------------------------------------------------------------
Rvector6 OrbitState::ConvertKeplerianToCartesian(Real a,  Real e,
                                                 Real i,  Real Om,
                                                 Real om, Real nu)
{
   // MATLAB notes: C++ code should be borrowed from GMAT state converter
   // To start, coding as is.  To use GMAT conversion utility,
   // we will need other body data (flattening, etc.)
   Rvector6 kepl(a, e, i * GmatMathConstants::DEG_PER_RAD, 
                 Om * GmatMathConstants::DEG_PER_RAD,
                 om * GmatMathConstants::DEG_PER_RAD,
                 nu * GmatMathConstants::DEG_PER_RAD);
   // conversion utility requires degrees
   Rvector6 cartesian = StateConversionUtil::KeplerianToCartesian(mu, kepl);
   return cartesian;
}

//------------------------------------------------------------------------------
// Rvector6 ConvertCartesianToKeplerian(const Rvector6 &cart)
//------------------------------------------------------------------------------
/**
 * Converts the cartesian state to a keplerian state.
 * 
 * @param cart    cartesian state
 * 
 * @return the keplerian state as a 6-element vector
 *
 */
//------------------------------------------------------------------------------
Rvector6 OrbitState::ConvertCartesianToKeplerian(const Rvector6 &cart)
{
   Rvector6 kepl = StateConversionUtil::CartesianToKeplerian(mu, cart);
   // conversion utility reports degrees
   kepl[2] *= GmatMathConstants::RAD_PER_DEG;
   kepl[3] *= GmatMathConstants::RAD_PER_DEG;
   kepl[4] *= GmatMathConstants::RAD_PER_DEG;
   kepl[5] *= GmatMathConstants::RAD_PER_DEG;
   return kepl;
}

//------------------------------------------------------------------------------
//  OrbitState* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the OrbitState.
 *
 * @return clone of the OrbitState.
 */
//------------------------------------------------------------------------------
OrbitState* OrbitState::Clone() const
{
   return (new OrbitState(*this));
}


