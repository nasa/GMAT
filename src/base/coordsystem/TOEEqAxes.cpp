//$Id$
//------------------------------------------------------------------------------
//                                  TOEEqAxes
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
// Created: 2005/04/27
//
/**
 * Implementation of the TOEEqAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Planet.hpp"
#include "TOEEqAxes.hpp"
#include "InertialAxes.hpp"
#include "GmatConstants.hpp"
#include "TimeSystemConverter.hpp"


//#define DEBUG_TOEEQ

#ifdef DEBUG_TOEEQ
   #include "MessageInterface.hpp"
#endif

//---------------------------------
// static data
//---------------------------------
// none

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  TOEEqAxes(const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base TOEEqAxes structures
 * (default constructor).
 *
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
TOEEqAxes::TOEEqAxes(const std::string &itsName) :
InertialAxes("TOEEq",itsName)
{
   objectTypeNames.push_back("TOEEqAxes");
   parameterCount = TOEEqAxesParamCount;
}

//---------------------------------------------------------------------------
//  TOEEqAxes(const TOEEqAxes &toe);
//---------------------------------------------------------------------------
/**
 * Constructs base TOEEqAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param toe  TOEEqAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
TOEEqAxes::TOEEqAxes(const TOEEqAxes &toe) :
InertialAxes(toe)
{
}

//---------------------------------------------------------------------------
//  TOEEqAxes& operator=(const TOEEqAxes &toe)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TOEEqAxes structures.
 *
 * @param toe The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const TOEEqAxes& TOEEqAxes::operator=(const TOEEqAxes &toe)
{
   if (&toe == this)
      return *this;
   InertialAxes::operator=(toe);   
   return *this;
}

//---------------------------------------------------------------------------
//  ~TOEEqAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
TOEEqAxes::~TOEEqAxes()
{
}

//---------------------------------------------------------------------------
//  bool TOEEqAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this TOEEqAxes.
 *
 * @return success flag
 */
//---------------------------------------------------------------------------
bool TOEEqAxes::Initialize()
{
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("Entering TOEEq::Initialize ..........\n");
   #endif
   InertialAxes::Initialize();
   InitializeFK5();
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("Completed IA:Init and InitFK5  ..........\n");
   #endif
   
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
   
   if (overrideOriginInterval) updateIntervalToUse = 
                               ((Planet*) origin)->GetNutationUpdateInterval();
   else                        updateIntervalToUse = updateInterval;
   
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("About to call ComputePrecession/Nutation ..........\n");
   #endif
   ComputePrecessionMatrix(tTDB, epoch);
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("Called ComputePrecession ..........\n");
   #endif
   ComputeNutationMatrix(tTDB, epoch, dPsi, longAscNodeLunar, cosEpsbar, true);
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("Called ComputeNutation ..........\n");
      if (precData == NULL)
         MessageInterface::ShowMessage("precData is NULL!!!!\n");
      if (nutData == NULL)
         MessageInterface::ShowMessage("nutData is NULL!!!!\n");
   #endif
   
   Real PrecT[9] = {precData[0], precData[3], precData[6],
                    precData[1], precData[4], precData[7],
                    precData[2], precData[5], precData[8]};
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("PrecT omputed  ..........\n");
   #endif
   Real NutT[9] =  {nutData[0], nutData[3], nutData[6],
                    nutData[1], nutData[4], nutData[7],
                    nutData[2], nutData[5], nutData[8]};
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("NutT computed  ..........\n");
   #endif

   Integer p3 = 0;
   Real res[3][3];
   for (Integer p = 0; p < 3; ++p)
   {
      p3 = 3*p;
      for (Integer q = 0; q < 3; ++q)
      {
         res[p][q] = PrecT[p3]   * NutT[q]   + 
                     PrecT[p3+1] * NutT[q+3] + 
                     PrecT[p3+2] * NutT[q+6];
      }
   }     
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("About to set rotMatrix ..........\n");
   #endif
   rotMatrix.Set(res[0][0],res[0][1],res[0][2],
                 res[1][0],res[1][1],res[1][2],
                 res[2][0],res[2][1],res[2][2]); 
   
   // rotDotMatrix is still the default zero matrix
   
   #ifdef DEBUG_TOEEQ
      MessageInterface::ShowMessage("EXITing TOEEq::Initialize ..........\n");
   #endif
   return true;
}


//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesEpoch() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage TOEEqAxes::UsesEpoch() const
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
GmatCoordinate::ParameterUsage TOEEqAxes::UsesEopFile(const std::string &forBaseSystem) const
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
GmatCoordinate::ParameterUsage TOEEqAxes::UsesItrfFile() const
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
GmatCoordinate::ParameterUsage TOEEqAxes::UsesNutationUpdateInterval() const
{
   //if (originName == SolarSystem::EARTH_NAME) 
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
 * This method returns a clone of the TOEEqAxes.
 *
 * @return clone of the TOEEqAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* TOEEqAxes::Clone() const
{
   return (new TOEEqAxes(*this));
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
void TOEEqAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation)
{
   // already computed in Initialize
}
