//------------------------------------------------------------------------------
//                           Attitude
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
 * Implementation of the base Attitude class
 */
//------------------------------------------------------------------------------

#include <cmath>            // for INFINITY
#include "gmatdefs.hpp"
#include "Attitude.hpp"
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
// Attitude()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 *
 */
//---------------------------------------------------------------------------
Attitude::Attitude()
{
}

//------------------------------------------------------------------------------
// Attitude(const Attitude &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 *
 * @param copy  the Attitude object to copy
 *
 */
//---------------------------------------------------------------------------
Attitude::Attitude(const Attitude &copy)
{
}

//------------------------------------------------------------------------------
// Attitude& operator=(const Attitude &copy)
//------------------------------------------------------------------------------
/**
 * The operator= for Attitude.
 *
 * @param copy  the Attitude object to copy
 *
 */
//---------------------------------------------------------------------------
Attitude& Attitude::operator=(const Attitude &copy)
{
   if (&copy == this)
      return *this;
   
   return *this;
}

//------------------------------------------------------------------------------
// ~Attitude()
//------------------------------------------------------------------------------
/**
 * Destructor.
 *
 */
//---------------------------------------------------------------------------
Attitude::~Attitude()
{
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
 * @return matrix from inertial to reference
 *
 * @note This method is expected to be implemented in child classes.
 *
 */
//---------------------------------------------------------------------------
Rmatrix33 Attitude::InertialToReference(const Rvector6& centralBodyState)
{
   Rmatrix33 I; // identity by default
   return I;
}

