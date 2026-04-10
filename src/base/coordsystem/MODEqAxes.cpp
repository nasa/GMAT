//$Id$
//------------------------------------------------------------------------------
//                                  MODEqAxes
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under 
// MOMS Task order 124.
//
// Author: Wendy C. Shoan
// Created: 2005/05/03
//
/**
 * Implementation of the MODEqAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Planet.hpp"
#include "MODEqAxes.hpp"
#include "MeanOfDateAxes.hpp"
#include "GmatConstants.hpp"
#include "TimeSystemConverter.hpp"


//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  MODEqAxes(const std::string &itsName);
//------------------------------------------------------------------------------
/**
 * Constructs base MODEqAxes structures
 * (default constructor).
 *
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//------------------------------------------------------------------------------
MODEqAxes::MODEqAxes(const std::string &itsName) :
MeanOfDateAxes("MODEq",itsName)
{
   objectTypeNames.push_back("MODEqAxes");
   parameterCount = MODEqAxesParamCount;
}

//------------------------------------------------------------------------------
//  MODEqAxes(const MODEqAxes &mod);
//------------------------------------------------------------------------------
/**
 * Constructs base MODEqAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param mod  MODEqAxes instance to copy to create "this" instance.
 */
//------------------------------------------------------------------------------
MODEqAxes::MODEqAxes(const MODEqAxes &mod) :
MeanOfDateAxes(mod)
{
}

//------------------------------------------------------------------------------
//  MODEqAxes& operator=(const MODEqAxes &mod)
//------------------------------------------------------------------------------
/**
 * Assignment operator for MODEqAxes structures.
 *
 * @param mod The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
const MODEqAxes& MODEqAxes::operator=(const MODEqAxes &mod)
{
   if (&mod == this)
      return *this;
   MeanOfDateAxes::operator=(mod);   
   return *this;
}

//------------------------------------------------------------------------------
//  ~MODEqAxes()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
MODEqAxes::~MODEqAxes()
{
}

//------------------------------------------------------------------------------
//  bool MODEqAxes::Initialize()
//------------------------------------------------------------------------------
/**
 * Initialization method for this MODEqAxes.
 *
 * @return success flag
 */
//------------------------------------------------------------------------------
bool MODEqAxes::Initialize()
{
   MeanOfDateAxes::Initialize();
   return true;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MODEqAxes.
 *
 * @return clone of the MODEqAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MODEqAxes::Clone() const
{
   return (new MODEqAxes(*this));
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch,
//                               bool forceComputation = false)
//------------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000EqAxes system.
 *
 * @param atEpoch          epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it is not time to do it
 *                         (default is false)
 */
//------------------------------------------------------------------------------
void MODEqAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation)
{
   
   // convert epoch (A1 MJD) to TT MJD (for calculations)
   Real mjdTT = theTimeConverter->Convert(atEpoch.Get(),
                TimeSystemConverter::A1MJD, TimeSystemConverter::TTMJD,
                GmatTimeConstants::JD_JAN_5_1941);      

   // Compute Julian centuries of TDB from the base epoch (J2000) 
   Real offset = GmatTimeConstants::JD_JAN_5_1941 - GmatTimeConstants::JD_OF_J2000;
   Real tTDB   = (mjdTT + offset) / GmatTimeConstants::DAYS_PER_JULIAN_CENTURY;
   
   if (overrideOriginInterval) updateIntervalToUse = 
                               ((Planet*) origin)->GetNutationUpdateInterval();
   else                        updateIntervalToUse = updateInterval;

   ComputePrecessionMatrix(tTDB, atEpoch);
   
   //rotMatrix = PREC.Transpose();
   rotMatrix.Set(precData[0], precData[3], precData[6],
                 precData[1], precData[4], precData[7],
                 precData[2], precData[5], precData[8]);
                 
   // rotDotMatrix is still the default zero matrix
   // (assume it is negligibly small)
}
