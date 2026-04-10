//$Id$
//------------------------------------------------------------------------------
//                                  TEMEAxes
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
//
// Author: Jairus Elarbee
// Created: 2021/10/04
//
/**
 * Implementation of the TEMEAxes class, based on 3-90 of Date TEME in Vallado 
 * Fundamentals of Astrodynamics and applications, 4th Edition
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Planet.hpp"
#include "TEMEAxes.hpp"
#include "TrueOfDateAxes.hpp"
#include "GmatConstants.hpp"
#include "TimeSystemConverter.hpp"
#include "RealUtilities.hpp"
#include "AngleUtil.hpp"
#include "CoordinateSystemException.hpp"

using namespace GmatMathUtil;           // for trig functions, etc.
using namespace GmatMathConstants;      // for RAD_PER_ARCSEC, etc.
using namespace GmatTimeConstants;      // for SECS_PER_DAY
//#define DEBUG_TEME_AXES

#ifdef DEBUG_TEME_AXES
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
//  TEMEAxes(const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base TEMEAxes structures
 * (default constructor).
 *
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
TEMEAxes::TEMEAxes(const std::string &itsName) :
TrueOfDateAxes("TEME",itsName)
{
   objectTypeNames.push_back("TEMEAxes");
   parameterCount = TEMEAxesParamCount;
}

//---------------------------------------------------------------------------
//  TEMEAxes(const TEMEAxes &tod);
//---------------------------------------------------------------------------
/**
 * Constructs base TEMEAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param tod  TEMEAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
TEMEAxes::TEMEAxes(const TEMEAxes &tod) :
TrueOfDateAxes(tod)
{
}

//---------------------------------------------------------------------------
//  TEMEAxes& operator=(const TEMEAxes &tod)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TEMEAxes structures.
 *
 * @param tod The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const TEMEAxes& TEMEAxes::operator=(const TEMEAxes &tod)
{
   if (&tod == this)
      return *this;
   TrueOfDateAxes::operator=(tod);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~TEMEAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
TEMEAxes::~TEMEAxes()
{
}

//---------------------------------------------------------------------------
//  bool TEMEAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this TEMEAxes.
 *
 * @return success flag
 */
//---------------------------------------------------------------------------
bool TEMEAxes::Initialize()
{
   TrueOfDateAxes::Initialize();
   // InitializeFK5() done in TrueOfDataAxes
   
   return true;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the TEMEAxes.
 *
 * @return clone of the TEMEAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* TEMEAxes::Clone() const
{
   return (new TEMEAxes(*this));
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
void TEMEAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation)
{
   #ifdef DEBUG_TEME_AXES
      MessageInterface::ShowMessage("Entering TEME::Calculate with epoch = %.12f\n",
         (Real) atEpoch.Get());
   #endif
   Real dPsi             = 0.0;
   Real longAscNodeLunar = 0.0;
   Real cosEpsbar        = 0.0;
   
   // convert epoch (A1 MJD) to TT MJD (for calculations)
   Real mjdTT = theTimeConverter->Convert(atEpoch.Get(),
                TimeSystemConverter::A1MJD, TimeSystemConverter::TTMJD,
                GmatTimeConstants::JD_JAN_5_1941);      
   Real offset = GmatTimeConstants::JD_JAN_5_1941 - GmatTimeConstants::JD_OF_J2000;
   Real tTDB  = (mjdTT + offset) / GmatTimeConstants::DAYS_PER_JULIAN_CENTURY;
   
   if (overrideOriginInterval) updateIntervalToUse = 
                               ((Planet*) origin)->GetNutationUpdateInterval();
   else                        updateIntervalToUse = updateInterval;
   #ifdef DEBUG_TEME_AXES
      MessageInterface::ShowMessage(
         "In TEME::Calculate tTDB = %.12f and updateIntervalToUse = %.12f\n",
         tTDB, updateIntervalToUse);
   #endif
   
   ComputePrecessionMatrix(tTDB, atEpoch);
   ComputeNutationMatrix(tTDB, atEpoch, dPsi, longAscNodeLunar, cosEpsbar,
                         forceComputation);
   Real eqMod = ComputeModEq(atEpoch.GetReal(),tTDB, dPsi, longAscNodeLunar, cosEpsbar);
   // See Vallado 3-91 for TEME->J2000
   // Compute Eq_Equinox1982


   
   #ifdef DEBUG_TEME_AXES
      MessageInterface::ShowMessage(
         "In TEME::Calculate precData = \n%.12f %.12f %.12f\n%.12f %.12f %.12f\n%.12f %.12f %.12f\n",
         precData[0],precData[1],precData[2],precData[3],precData[4],
         precData[5],precData[6],precData[7],precData[8]);
      MessageInterface::ShowMessage(
         "In TEME::Calculate nutData = \n%.12f %.12f %.12f\n%.12f %.12f %.12f\n%.12f %.12f %.12f\n",
         nutData[0],nutData[1],nutData[2],nutData[3],nutData[4],
         nutData[5],nutData[6],nutData[7],nutData[8]);
   #endif
   
   Real PrecT[9] = {precData[0], precData[3], precData[6],
                    precData[1], precData[4], precData[7],
                    precData[2], precData[5], precData[8]};
   Real NutT[9] =  {nutData[0], nutData[3], nutData[6],
                    nutData[1], nutData[4], nutData[7],
                    nutData[2], nutData[5], nutData[8]};
   Real EqNox[9] = { cos(-eqMod), sin(-eqMod), 0,
                    -sin(-eqMod), cos(-eqMod), 0,
                    0, 0, 1};

   Integer p3 = 0;
   Real temp[3][3];
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

   for (Integer p = 0; p < 3; ++p)
   {
      for (Integer q = 0; q < 3; ++q)
      {
         temp[p][q] = res[p][0] * EqNox[q] +
            res[p][1] * EqNox[q + 3] +
            res[p][2] * EqNox[q + 6];
      }
   }
   rotMatrix.Set(temp[0][0], temp[0][1], temp[0][2],
      temp[1][0], temp[1][1], temp[1][2],
      temp[2][0], temp[2][1], temp[2][2]);

   // rotDotMatrix is still the default zero matrix 
   // (assume it is negligibly small)
}


//------------------------------------------------------------------------------
//  void ComputeSiderealTimeDotRotation(const GmatTime &mjdUTC,
//                                      const GmatTime &atEpoch,
//                                      Real &cosAst,
//                                      Real &sinAst,
//                                      bool forceComputation)
//------------------------------------------------------------------------------
/**
 * This method will compute the Sidereal time dot rotation matrix.
 *
 * @param mjdUTC            UTC mean julian day
 * @param atEpoch           A1Mjd epoch
 * @param cosAst            cos quantity
 * @param sinAst            sin quantity
 * @param forceComputation  force matrix computation?
 */
 //------------------------------------------------------------------------------
Real TEMEAxes::ComputeModEq(Real a1Epoch,
   Real tUT1,
   Real dPsi,
   Real longAscNodeLunar,
   Real cosEpsbar)
{

   // Compute elapsed Julian centuries (UT1)
   //Real tDiff = JD_JAN_5_1941 - JD_OF_J2000;
   //Real tUT1 = (mjdUT1 + tDiff).GetMjd() / DAYS_PER_JULIAN_CENTURY;

   Real tUT12 = tUT1 * tUT1;
   Real tUT13 = tUT12 * tUT1;

   // First, compute the equation of the equinoxes
   // If the time is before January 1, 1997, don't use the last two terms
   Real term2 = 0.0;
   Real term3 = 0.0;
  
   Real EQequinox = (dPsi * cosEpsbar) + term2 + term3;

   return EQequinox;
}


//------------------------------------------------------------------------------
// void TEMEAxes::InitializeReference(SpacePoint *refObj)
//------------------------------------------------------------------------------
/**
 * TEME requires Earth as the origin.  That is trapped here.
 *
 * @param refObj The reference body for the TEME axes
 */
//------------------------------------------------------------------------------
void TEMEAxes::InitializeReference(SpacePoint *refObj)
{
   if (refObj->GetName() != "Earth")
      throw CoordinateSystemException("TEME axes are only defined for the Earth");

   TrueOfDateAxes::InitializeReference(refObj);
}
