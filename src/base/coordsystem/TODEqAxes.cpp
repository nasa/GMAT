//$Id$
//------------------------------------------------------------------------------
//                                  TODEqAxes
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
 * Implementation of the TODEqAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "Planet.hpp"
#include "TODEqAxes.hpp"
#include "TrueOfDateAxes.hpp"
#include "GmatConstants.hpp"
#include "TimeSystemConverter.hpp"

//#define DEBUG_TODEQ_AXES

#ifdef DEBUG_TODEQ_AXES
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
//  TODEqAxes(const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base TODEqAxes structures
 * (default constructor).
 *
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
TODEqAxes::TODEqAxes(const std::string &itsName) :
TrueOfDateAxes("TODEq",itsName)
{
   objectTypeNames.push_back("TODEqAxes");
   parameterCount = TODEqAxesParamCount;
}

//---------------------------------------------------------------------------
//  TODEqAxes(const TODEqAxes &tod);
//---------------------------------------------------------------------------
/**
 * Constructs base TODEqAxes structures used in derived classes, by copying 
 * the input instance (copy constructor).
 *
 * @param tod  TODEqAxes instance to copy to create "this" instance.
 */
//---------------------------------------------------------------------------
TODEqAxes::TODEqAxes(const TODEqAxes &tod) :
TrueOfDateAxes(tod)
{
}

//---------------------------------------------------------------------------
//  TODEqAxes& operator=(const TODEqAxes &tod)
//---------------------------------------------------------------------------
/**
 * Assignment operator for TODEqAxes structures.
 *
 * @param tod The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const TODEqAxes& TODEqAxes::operator=(const TODEqAxes &tod)
{
   if (&tod == this)
      return *this;
   TrueOfDateAxes::operator=(tod);   
   return *this;
}
//---------------------------------------------------------------------------
//  ~TODEqAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
TODEqAxes::~TODEqAxes()
{
}

//---------------------------------------------------------------------------
//  bool TODEqAxes::Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this TODEqAxes.
 *
 * @return success flag
 */
//---------------------------------------------------------------------------
bool TODEqAxes::Initialize()
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
 * This method returns a clone of the TODEqAxes.
 *
 * @return clone of the TODEqAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* TODEqAxes::Clone() const
{
   return (new TODEqAxes(*this));
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
void TODEqAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                        bool forceComputation)
{
   #ifdef DEBUG_TODEQ_AXES
      MessageInterface::ShowMessage("Entering TODEQ::Calculate with epoch = %.12f\n",
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
   #ifdef DEBUG_TODEQ_AXES
      MessageInterface::ShowMessage(
         "In TODEQ::Calculate tTDB = %.12f and updateIntervalToUse = %.12f\n",
         tTDB, updateIntervalToUse);
   #endif
   
   ComputePrecessionMatrix(tTDB, atEpoch);
   ComputeNutationMatrix(tTDB, atEpoch, dPsi, longAscNodeLunar, cosEpsbar,
                         forceComputation);
   
   #ifdef DEBUG_TODEQ_AXES
      MessageInterface::ShowMessage(
         "In TODEQ::Calculate precData = \n%.12f %.12f %.12f\n%.12f %.12f %.12f\n%.12f %.12f %.12f\n",
         precData[0],precData[1],precData[2],precData[3],precData[4],
         precData[5],precData[6],precData[7],precData[8]);
      MessageInterface::ShowMessage(
         "In TODEQ::Calculate nutData = \n%.12f %.12f %.12f\n%.12f %.12f %.12f\n%.12f %.12f %.12f\n",
         nutData[0],nutData[1],nutData[2],nutData[3],nutData[4],
         nutData[5],nutData[6],nutData[7],nutData[8]);
   #endif
   
   Real PrecT[9] = {precData[0], precData[3], precData[6],
                    precData[1], precData[4], precData[7],
                    precData[2], precData[5], precData[8]};
   Real NutT[9] =  {nutData[0], nutData[3], nutData[6],
                    nutData[1], nutData[4], nutData[7],
                    nutData[2], nutData[5], nutData[8]};

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
   rotMatrix.Set(res[0][0],res[0][1],res[0][2],
                 res[1][0],res[1][1],res[1][2],
                 res[2][0],res[2][1],res[2][2]); 

   // rotDotMatrix is still the default zero matrix 
   // (assume it is negligibly small)
}
