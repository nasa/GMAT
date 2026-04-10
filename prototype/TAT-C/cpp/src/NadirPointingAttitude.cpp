//------------------------------------------------------------------------------
//                           NadirPointingAttitude
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
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
// Created: 2018.08.01
//
/**
 * Implementation of the base NadirPointingAttitude class
 */
//------------------------------------------------------------------------------

#include <cmath>            // for INFINITY
#include "gmatdefs.hpp"
#include "Rvector3.hpp"
#include "NadirPointingAttitude.hpp"
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
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
// NadirPointingAttitude()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 *
 */
//------------------------------------------------------------------------------
NadirPointingAttitude::NadirPointingAttitude() :
   Attitude()
{
}

//------------------------------------------------------------------------------
// NadirPointingAttitude(const NadirPointingAttitude &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param copy  the NadirPointingAttitude object to copy
 *
 */
//------------------------------------------------------------------------------
NadirPointingAttitude::NadirPointingAttitude(
                       const NadirPointingAttitude &copy) :
   Attitude(copy)
{
}

//------------------------------------------------------------------------------
// NadirPointingAttitude& operator=(const NadirPointingAttitude &copy)
//------------------------------------------------------------------------------
/**
 * The operator= for NadirPointingAttitude.
 *
 * @param copy  the NadirPointingAttitude object to copy
 *
 */
//------------------------------------------------------------------------------
NadirPointingAttitude& NadirPointingAttitude::operator=(const NadirPointingAttitude &copy)
{
   if (&copy == this)
      return *this;
   
   Attitude::operator=(copy);
      
   return *this;
}

//------------------------------------------------------------------------------
// ~NadirPointingAttitude()
//------------------------------------------------------------------------------
/**
 * Destructor.
 *
 */
//---------------------------------------------------------------------------
NadirPointingAttitude::~NadirPointingAttitude()
{
}

//------------------------------------------------------------------------------
//  Attitude* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of this Attitude.
 *
 * @return clone of the Attitude class.
 */
//------------------------------------------------------------------------------
Attitude* NadirPointingAttitude::Clone() const
{
   return (new NadirPointingAttitude(*this));
}


//------------------------------------------------------------------------------
// Rmatrix33 InertialToReference(const Rvector6& centralBodyState)
//------------------------------------------------------------------------------
/**
 * This method computes the matrix that converts from inertial to the reference
 * frame, given the input central body state
 *
 * @param centralBodyState  central body state
 *
 * @return matrix from body to inertial
 *
 */
//------------------------------------------------------------------------------
Rmatrix33 NadirPointingAttitude::InertialToReference(const Rvector6& centralBodyState)
{
   centralBodyFixedPos.Set(centralBodyState[0],
                           centralBodyState[1],
                           centralBodyState[2]);
   centralBodyFixedVel.Set(centralBodyState[3],
                           centralBodyState[4],
                           centralBodyState[5]);
   zHat = -centralBodyFixedPos;
   zHat.Normalize();
   xHat = Cross(zHat, centralBodyFixedVel);
   xHat = -xHat.Normalize();
   yHat = Cross(zHat, xHat);
   
   R_fixed_to_nadir.Set(xHat[0], yHat[0], zHat[0],
                        xHat[1], yHat[1], zHat[1],
                        xHat[2], yHat[2], zHat[2]);
   R_fixed_to_nadir_transposed = R_fixed_to_nadir.Transpose();
   return R_fixed_to_nadir_transposed;
}

