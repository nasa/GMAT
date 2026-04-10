//$Id$
//------------------------------------------------------------------------------
//                                  BodyInertialAxes
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
// Created: 2006.08.01
//
/**
 * Implementation of the BodyInertialAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "BodyInertialAxes.hpp"
#include "InertialAxes.hpp"
#include "GmatConstants.hpp"
#include "Rvector.hpp"
#include "RealUtilities.hpp"
#include "TimeSystemConverter.hpp"
#include "CoordinateSystemException.hpp"

//#define DEBUG_BODY_INERTIAL

#ifdef DEBUG_BODY_INERTIAL
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
//  BodyInertialAxes(const std::string &itsType,
//                   const std::string &itsName);
//---------------------------------------------------------------------------
/**
 * Constructs base BodyInertialAxes structures
 * (default constructor).
 *
 * @param <itsType> GMAT script string associated with this type of object.
 * @param <itsName> Optional name for the object.  Defaults to "".
 *
 */
//---------------------------------------------------------------------------
BodyInertialAxes::BodyInertialAxes(const std::string &itsName) :
InertialAxes("BodyInertial",itsName)
{
   objectTypeNames.push_back("BodyInertialAxes");
   parameterCount = BodyInertialAxesParamCount;
   Real TAIModJul   = 2451544.9996274998411 - GmatTimeConstants::JD_JAN_5_1941;
   Real mjdA1       = theTimeConverter->Convert(TAIModJul,
                      TimeSystemConverter::TAIMJD, TimeSystemConverter::A1MJD,
                      GmatTimeConstants::JD_JAN_5_1941); 
   epoch.Set(mjdA1);
   needsCBOrigin = true;
   
   #ifdef DEBUG_BODY_INERTIAL
      MessageInterface::ShowMessage("BodyInertialAxes created with name %s\n",
      instanceName.c_str());
   #endif
}

//---------------------------------------------------------------------------
//  BodyInertialAxes(const BodyInertialAxes &bodyInertial);
//---------------------------------------------------------------------------
/**
 * Constructs base BodyInertialAxes structures used in derived classes, by 
 * copying the input instance (copy constructor).
 *
 * @param bodyInertial  BodyInertialAxes instance to copy to create "this" 
 *                                       instance.
 */
//---------------------------------------------------------------------------
BodyInertialAxes::BodyInertialAxes(const BodyInertialAxes &bodyInertial) :
InertialAxes(bodyInertial)
{
   #ifdef DEBUG_BODY_INERTIAL
      MessageInterface::ShowMessage("BodyInertialAxes created (copied) with name %s\n",
      instanceName.c_str());
   #endif
}

//---------------------------------------------------------------------------
//  BodyInertialAxes& operator=(const BodyInertialAxes &bodyInertial)
//---------------------------------------------------------------------------
/**
 * Assignment operator for BodyInertialAxes structures.
 *
 * @param bodyInertial The original that is being copied.
 *
 * @return Reference to this object
 */
//---------------------------------------------------------------------------
const BodyInertialAxes& BodyInertialAxes::operator=(const BodyInertialAxes &bodyInertial)
{
   if (&bodyInertial == this)
      return *this;
   InertialAxes::operator=(bodyInertial);   
   #ifdef DEBUG_BODY_INERTIAL
      MessageInterface::ShowMessage("BodyInertialAxes created (operator=) with name %s\n",
      instanceName.c_str());
   #endif
   return *this;
}
//---------------------------------------------------------------------------
//  ~BodyInertialAxes()
//---------------------------------------------------------------------------
/**
 * Destructor.
 */
//---------------------------------------------------------------------------
BodyInertialAxes::~BodyInertialAxes()
{
}

//---------------------------------------------------------------------------
//  bool Initialize()
//---------------------------------------------------------------------------
/**
 * Initialization method for this BodyInertialAxes.  Creates the rotation 
 * matrix, based on IAU data (except for hard-coded values for 1) Earth, 
 * which uses the FK5 value at epoch; and 2) Luna, which is based on a
 * DE405-supplied orientation at epoch).
 *
 */
//---------------------------------------------------------------------------
bool BodyInertialAxes::Initialize()
{
   InertialAxes::Initialize();
   #ifdef DEBUG_BODY_INERTIAL
      MessageInterface::ShowMessage("Initializing BodyInertialAxes with name \"%s\"\n",
      instanceName.c_str());
      MessageInterface::ShowMessage("   originName = %s\n", originName.c_str());
   #endif
   // if origin is not a CelestialBody, there is an error
   // this should be caught at the CoordinateBase level
   if (!(origin->IsOfType("CelestialBody")))
      throw CoordinateSystemException(
            "Improper origin set for BodyInertialAxes object.");
   //if (originName == SolarSystem::EARTH_NAME)
   if (originName == GmatSolarSystemDefaults::EARTH_NAME)
   {
      rotMatrix.Set(1.0,0.0,0.0,
                    0.0,1.0,0.0,
                    0.0,0.0,1.0);
   }
   //else if (originName == SolarSystem::MOON_NAME)
   else if (originName == GmatSolarSystemDefaults::MOON_NAME)
   {
      rotMatrix.Set( 0.998496505205088,  0.0499357293985327, -0.0226086714041825,
                    -0.0548154092680678, 0.909610125238044,  -0.411830900942613,
                     0.0,                0.412451018902689,   0.91097977859343   );
   }
   else
   {
      // get alpha, delta, W, Wdot at the A1Mjd time of epoch
      Rvector   coords(4);
      coords              = ((CelestialBody*)origin)->GetBodyCartographicCoordinates(epoch);
      #ifdef DEBUG_BODY_INERTIAL
         MessageInterface::ShowMessage("   Body cartographic coords for body %s are %12.10f   %12.10f   %12.10f   %12.10f\n",
               (origin->GetName()).c_str(), coords[0], coords[1], coords[2], coords[3]);
      #endif
      Real      a         = GmatMathConstants::PI_OVER_TWO +    // 90 + alpha
                            GmatMathUtil::Rad(coords[0]);
      Real      b         = GmatMathConstants::PI_OVER_TWO -    // 90 - delta
                            GmatMathUtil::Rad(coords[1]);
      Rmatrix33 R3T( GmatMathUtil::Cos(a), -GmatMathUtil::Sin(a), 0.0,
                     GmatMathUtil::Sin(a),  GmatMathUtil::Cos(a), 0.0,
                                      0.0,                   0.0, 1.0);
      Rmatrix33 R1T( 1.0,                   0.0,                   0.0,
                     0.0,  GmatMathUtil::Cos(b), -GmatMathUtil::Sin(b),
                     0.0,  GmatMathUtil::Sin(b),  GmatMathUtil::Cos(b));
      rotMatrix = R3T * R1T;
   }
   #ifdef DEBUG_BODY_INERTIAL
      MessageInterface::ShowMessage("At end of BodyInertialAxes::Initialize, rotMatrix    = %s\n", (rotMatrix.ToString()).c_str());
      MessageInterface::ShowMessage("                                        rotDotMatrix = %s\n", (rotDotMatrix.ToString()).c_str());
   #endif
   // rotDotMatrix remains the zero matrix
   return true;
}


//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the BodyInertialAxes.
 *
 * @return clone of the BodyInertialAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* BodyInertialAxes::Clone() const
{
   return (new BodyInertialAxes(*this));
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
 * @param forceComputation force computation even if it's not time to do it
 *                         (default is false)
 */
//---------------------------------------------------------------------------
void BodyInertialAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                               bool forceComputation)
{
   // already computed in Initialize
}
