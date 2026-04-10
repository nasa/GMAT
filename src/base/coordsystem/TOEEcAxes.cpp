//$Id$
//------------------------------------------------------------------------------
//                                  TOEEcAxes
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
 * Implementation of the TOEEcAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Planet.hpp"
#include "TOEEcAxes.hpp"
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
//  TOEEcAxes(const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base TOEEcAxes structures
 * (default constructor).
 *
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
TOEEcAxes::TOEEcAxes(const std::string &itsName) :
InertialAxes("TOEEc",itsName)
{
   objectTypeNames.push_back("TOEEcAxes");
   parameterCount = TOEEcAxesParamCount;
}

//---------------------------------------------------------------------------
//  TOEEcAxes(const TOEEcAxes &toe);
//---------------------------------------------------------------------------
/**
 * Constructs base TOEEcAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param toe  TOEEcAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
TOEEcAxes::TOEEcAxes(const TOEEcAxes &toe) :
InertialAxes(toe)
{
}

//---------------------------------------------------------------------------
//  TOEEcAxes& operator=(const TOEEcAxes &toe)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TOEEcAxes structures.
 *
 * @param toe The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const TOEEcAxes& TOEEcAxes::operator=(const TOEEcAxes &toe)
{
   if (&toe == this)
      return *this;
   InertialAxes::operator=(toe);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~TOEEcAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
TOEEcAxes::~TOEEcAxes()
{
}

//---------------------------------------------------------------------------
//  bool TOEEcAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this TOEEcAxes.
 *
 * @return success flag
 */
//---------------------------------------------------------------------------
bool TOEEcAxes::Initialize()
{
   InertialAxes::Initialize();
   InitializeFK5();
   
   Real dPsi             = 0.0;
   Real longAscNodeLunar = 0.0;
   Real cosEpsbar        = 0.0;

   // convert epoch (A1 MJD) to TT MJD (for calculations)
   Real mjdTT = theTimeConverter->Convert(epoch.Get(),
                 TimeSystemConverter::A1MJD, TimeSystemConverter::TTMJD,
                 GmatTimeConstants::JD_JAN_5_1941);      

   // Compute Julian centuries of TDB from the base epoch (J2000) 
   Real offset = GmatTimeConstants::JD_JAN_5_1941 - GmatTimeConstants::JD_OF_J2000;
   Real tTDB  = (mjdTT + offset) / GmatTimeConstants::DAYS_PER_JULIAN_CENTURY;
   Real tTDB2 = tTDB * tTDB;
   Real tTDB3 = tTDB * tTDB2;
   
   // Vallado Eq. 3-52
   Real Epsbar  = (84381.448 - 46.8150*tTDB - 0.00059*tTDB2 + 0.001813*tTDB3)
      * GmatMathConstants::RAD_PER_ARCSEC;
   Real R1EpsT[9] = { 1.0,                      0.0,                       0.0,
                      0.0,GmatMathUtil::Cos(Epsbar),-GmatMathUtil::Sin(Epsbar),
                      0.0,GmatMathUtil::Sin(Epsbar), GmatMathUtil::Cos(Epsbar)};
   
   
   if (overrideOriginInterval) updateIntervalToUse = 
                               ((Planet*) origin)->GetNutationUpdateInterval();
   else                        updateIntervalToUse = updateInterval;

   ComputePrecessionMatrix(tTDB, epoch);
   ComputeNutationMatrix(tTDB, epoch, dPsi, longAscNodeLunar, cosEpsbar, true);

   Real R3PsiT[3][3] = { { GmatMathUtil::Cos(-dPsi),-GmatMathUtil::Sin(-dPsi), 0.0},
                         { GmatMathUtil::Sin(-dPsi), GmatMathUtil::Cos(-dPsi), 0.0},
                         {                      0.0,                     0.0,  1.0}};
   Real PrecT[9] = {precData[0], precData[3], precData[6],
                    precData[1], precData[4], precData[7],
                    precData[2], precData[5], precData[8]};
   
   
   Real res[3][3], tmp[3][3];
   Integer p3 = 0;
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      for (Integer q = 0; q < 3; ++q)
      {
         tmp[p][q] = R1EpsT[p3]   * R3PsiT[0][q] + 
                     R1EpsT[p3+1] * R3PsiT[1][q] + 
                     R1EpsT[p3+2] * R3PsiT[2][q];
      }
   }     
    for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      for (Integer q = 0; q < 3; ++q)
      {
         res[p][q] = PrecT[p3]   * tmp[0][q] + 
                     PrecT[p3+1] * tmp[1][q] + 
                     PrecT[p3+2] * tmp[2][q];
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
GmatCoordinate::ParameterUsage TOEEcAxes::UsesEpoch() const
{
   return GmatCoordinate::REQUIRED;
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesEopFile(
//                                 const std::string &forBaseSystem) const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage TOEEcAxes::UsesEopFile(const std::string &forBaseSystem) const
{
   if (forBaseSystem == baseSystem)
      return GmatCoordinate::REQUIRED;
   return GmatCoordinate::NOT_USED;
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesItrfFile() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage TOEEcAxes::UsesItrfFile() const
{
   return GmatCoordinate::REQUIRED;
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesNutationUpdateInterval() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage TOEEcAxes::UsesNutationUpdateInterval() const
{
   if (originName == GmatSolarSystemDefaults::EARTH_NAME)
      return GmatCoordinate::REQUIRED;
   return InertialAxes::UsesNutationUpdateInterval();
}



//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the TOEEcAxes.
 *
 * @return clone of the TOEEcAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* TOEEcAxes::Clone() const
{
   return (new TOEEcAxes(*this));
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
 * @param atEpoch  epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it is not time to do it
 *                         (default is false)
 */
//---------------------------------------------------------------------------
void TOEEcAxes::CalculateRotationMatrix(const A1Mjd &atEpoch, 
                                        bool forceComputation)
{
   // already computed in Initialize
}
