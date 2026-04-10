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
 * Definition of the OrbitState class.  This class computes and converts
 * Cartesian and Keplerian states.
 */
//------------------------------------------------------------------------------
#ifndef OrbitState_hpp
#define OrbitState_hpp

#include "gmatdefs.hpp"
#include "Rvector6.hpp"

class OrbitState
{
public:
   
   /// class construction/destruction
   OrbitState();
   OrbitState( const OrbitState &copy);
   OrbitState& operator=(const OrbitState &copy);
   
   virtual ~OrbitState();
   
   /// Set the Keplerian State elements
   virtual void     SetKeplerianState(Real SMA, Real ECC,
                                      Real INC, Real RAAN,
                                      Real AOP, Real TA);
   /// Set the Kerlerian state vector
   virtual void     SetKeplerianVectorState(const Rvector6 &kepl);
   /// Set the Cartesian state
   virtual void     SetCartesianState(const Rvector6 &cart);
   /// Set the gravity parameter
   virtual void     SetGravityParameter(Real toGrav);
   /// Return the Keplerian state
   virtual Rvector6 GetKeplerianState();
   /// Return the Cartesian state
   virtual Rvector6 GetCartesianState();
   
   /// Clone the OrbitState object
   virtual OrbitState* Clone() const;
protected:
   
   /// Current state in cartesian format
   Rvector6  currentState;
   /// Gravitational parameter for the central body
   Real      mu;       // units??
   
   /// State conversion methods
   Rvector6  ConvertKeplerianToCartesian(Real a,  Real e,
                                         Real i,  Real Om,
                                         Real om, Real nu);
   Rvector6  ConvertCartesianToKeplerian(const Rvector6 &cart);   
};
#endif // OrbitState_hpp
