//$Id$
//------------------------------------------------------------------------------
//                                  MOEEcAxes
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
// Created: 2005/05/11
//
/**
 * Implementation of the MOEEcAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Planet.hpp"
#include "MOEEcAxes.hpp"
#include "InertialAxes.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "TimeSystemConverter.hpp"


//---------------------------------
// static data
//---------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  MOEEcAxes(const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base MOEEcAxes structures
 * (default constructor).
 *
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
MOEEcAxes::MOEEcAxes(const std::string &itsName) :
InertialAxes("MOEEc",itsName)
{
   objectTypeNames.push_back("MOEEcAxes");
   parameterCount = MOEEcAxesParamCount;
}

//---------------------------------------------------------------------------
//  MOEEcAxes(const MOEEcAxes &moe);
//---------------------------------------------------------------------------
/**
 * Constructs base MOEEcAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param moe  MOEEcAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
MOEEcAxes::MOEEcAxes(const MOEEcAxes &moe) :
InertialAxes(moe)
{
}

//---------------------------------------------------------------------------
//  MOEEcAxes& operator=(const MOEEcAxes &moe)
//---------------------------------------------------------------------------
/**
 * Assignment operator for MOEEcAxes structures.
 *
 * @param moe The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const MOEEcAxes& MOEEcAxes::operator=(const MOEEcAxes &moe)
{
   if (&moe == this)
      return *this;
   InertialAxes::operator=(moe);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~MOEEcAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
MOEEcAxes::~MOEEcAxes()
{
}

//---------------------------------------------------------------------------
//  bool MOEEcAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this MOEEcAxes.
 *
 * @return success flag
 */
//---------------------------------------------------------------------------
bool MOEEcAxes::Initialize()
{
   InertialAxes::Initialize();

   // convert epoch (A1 MJD) to TT MJD (for calculations)
   Real mjdTT = theTimeConverter->Convert(epoch.Get(),
                 TimeSystemConverter::A1MJD, TimeSystemConverter::TTMJD,
                 GmatTimeConstants::JD_JAN_5_1941);      

   // Compute Julian centuries of TDB from the base epoch (J2000) 
   Real offset = GmatTimeConstants::JD_JAN_5_1941 - GmatTimeConstants::JD_OF_J2000;
   Real tTDB  = (mjdTT +offset) / GmatTimeConstants::DAYS_PER_JULIAN_CENTURY;
   Real tTDB2 = tTDB * tTDB;
   Real tTDB3 = tTDB * tTDB2;
   
   // Vallado Eq. 3-52
   Real Epsbar  = (84381.448 - 46.8150*tTDB - 0.00059*tTDB2 + 0.001813*tTDB3)
      * GmatMathConstants::RAD_PER_ARCSEC;
   
   Real R1EpsT[3][3] = { {1.0,                      0.0,                       0.0},
                         {0.0,GmatMathUtil::Cos(Epsbar),-GmatMathUtil::Sin(Epsbar)},
                         {0.0,GmatMathUtil::Sin(Epsbar), GmatMathUtil::Cos(Epsbar)}};
   
   if (overrideOriginInterval) updateIntervalToUse = 
                               ((Planet*) origin)->GetNutationUpdateInterval();
   else                        updateIntervalToUse = updateInterval;
   ComputePrecessionMatrix(tTDB, epoch);
   
   Real PrecT[9] = {precData[0], precData[3], precData[6],
                    precData[1], precData[4], precData[7],
                    precData[2], precData[5], precData[8]};
   
   //rotMatrix = PREC.Transpose() * R1Eps.Transpose();
   Real res[3][3];
   Integer p3 = 0;
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      for (Integer q = 0; q < 3; ++q)
      {
         res[p][q] = PrecT[p3]   * R1EpsT[0][q] + 
                     PrecT[p3+1] * R1EpsT[1][q] + 
                     PrecT[p3+2] * R1EpsT[2][q];
      }
   }     
   
   rotMatrix.Set(res[0][0],res[0][1],res[0][2],
                 res[1][0],res[1][1],res[1][2],
                 res[2][0],res[2][1],res[2][2]); 
   // rotDotMatrix is still the default zero matrix
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesEpoch() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage MOEEcAxes::UsesEpoch() const
{
   return GmatCoordinate::REQUIRED;
}

//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the MOEEcAxes.
 *
 * @return clone of the MOEEcAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* MOEEcAxes::Clone() const
{
   return (new MOEEcAxes(*this));
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  void CalculateRotationMatrix(const A1Mjd &atEpoch,
//                               bool forceComputation = false)
//---------------------------------------------------------------------------
/**
 * This method will compute the rotMatrix and rotDotMatrix used for rotations
 * from/to this AxisSystem to/from the MJ2000EqAxes system.
 *
 * @param atEpoch          epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it is not time to do it
 *                         (default is false)
 */
//---------------------------------------------------------------------------
void MOEEcAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation)
{
   // already computed in Initialize
}
