//$Id$
//------------------------------------------------------------------------------
//                                  GeocentricSolarEclipticAxes
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
// Author: Wendy C. Shoan/GSFC/MAB
// Created: 2005/06/02
//
/**
 * Implementation of the GeocentricSolarEclipticAxes class.  
 *
 */
//------------------------------------------------------------------------------

#include <iostream>
#include "gmatdefs.hpp"
#include "GmatBase.hpp"
#include "GmatBaseException.hpp"
#include "ObjectReferencedAxes.hpp"
#include "GeocentricSolarEclipticAxes.hpp"
#include "DynamicAxes.hpp"
#include "SolarSystem.hpp"
#include "CelestialBody.hpp"
#include "RealUtilities.hpp"
#include "Linear.hpp"
#include "TimeTypes.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "TimeSystemConverter.hpp"
#include "CoordinateSystemException.hpp"
#include "MessageInterface.hpp"

#include <iostream>

//#define DEBUG_GSE_CALC


using namespace GmatMathUtil;      // for trig functions, etc.
using namespace GmatTimeConstants;      // for JD offsets, etc.

//---------------------------------
// static data
//---------------------------------
// none


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  GeocentricSolarEclipticAxes(const std::string &itsName);
//------------------------------------------------------------------------------
/**
 * Constructs base GeocentricSolarEclipticAxes structures
 * (default constructor).
 *
 * @param itsName Optional name for the object.  Defaults to "".
 */
//------------------------------------------------------------------------------
GeocentricSolarEclipticAxes::GeocentricSolarEclipticAxes(const std::string &itsName) :
ObjectReferencedAxes("GSE",itsName)
{
   primaryName   = GmatSolarSystemDefaults::EARTH_NAME;
   secondaryName = GmatSolarSystemDefaults::SUN_NAME;
   objectTypeNames.push_back("GSE");
   parameterCount = GeocentricSolarEclipticAxesParamCount;

   usesPrimary   = GmatCoordinate::REQUIRED_UNMODIFIABLE;
   usesSecondary = GmatCoordinate::REQUIRED_UNMODIFIABLE;
}


//------------------------------------------------------------------------------
//  GeocentricSolarEclipticAxes(const GeocentricSolarEclipticAxes &gse);
//------------------------------------------------------------------------------
/**
 * Constructs base GeocentricSolarEclipticAxes structures used in derived classes, 
 * by copying the input instance (copy constructor).
 *
 * @param gse  GeocentricSolarEclipticAxes instance to copy to create "this" instance.
 */
//------------------------------------------------------------------------------
GeocentricSolarEclipticAxes::GeocentricSolarEclipticAxes(
                             const GeocentricSolarEclipticAxes &gse) :
ObjectReferencedAxes(gse)
{
}

//------------------------------------------------------------------------------
//  GeocentricSolarEclipticAxes& operator=(
//                               const GeocentricSolarEclipticAxes &gse)
//------------------------------------------------------------------------------
/**
 * Assignment operator for GeocentricSolarEclipticAxes structures.
 *
 * @param gse The original that is being copied.
 *
 * @return Reference to this object
 */
//------------------------------------------------------------------------------
const GeocentricSolarEclipticAxes& 
GeocentricSolarEclipticAxes::operator=(const GeocentricSolarEclipticAxes &gse)
{
   if (&gse == this)
      return *this;
   ObjectReferencedAxes::operator=(gse);
   return *this;
}
//------------------------------------------------------------------------------
//  ~GeocentricSolarEclipticAxes()
//------------------------------------------------------------------------------
/**
 * Destructor.
 */
//------------------------------------------------------------------------------
GeocentricSolarEclipticAxes::~GeocentricSolarEclipticAxes()
{
}

//---------------------------------------------------------------------------
//  bool IsParameterReadOnly(const Integer id) const
//---------------------------------------------------------------------------
/**
 * @see GmatBase
 */
//---------------------------------------------------------------------------
bool GeocentricSolarEclipticAxes::IsParameterReadOnly(const Integer id) const
{
   switch (id)
   {
      case PRIMARY_OBJECT_NAME:
      case SECONDARY_OBJECT_NAME:
      case X_AXIS:
      case Y_AXIS:
      case Z_AXIS:
         return true;
         break;
      default:
         return ObjectReferencedAxes::IsParameterReadOnly(id);
   }
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesXAxis() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage GeocentricSolarEclipticAxes::UsesXAxis() const
{
   return GmatCoordinate::NOT_USED;
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesYAxis() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage GeocentricSolarEclipticAxes::UsesYAxis() const
{
   return GmatCoordinate::NOT_USED;
}

//------------------------------------------------------------------------------
//  GmatCoordinate::ParameterUsage UsesZAxis() const
//------------------------------------------------------------------------------
/**
 * @see AxisSystem
 */
//---------------------------------------------------------------------------
GmatCoordinate::ParameterUsage GeocentricSolarEclipticAxes::UsesZAxis() const
{
   return GmatCoordinate::NOT_USED;
}

//------------------------------------------------------------------------------
// public methods inherited from GmatBase
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the GeocentricSolarEclipticAxes.
 *
 * @return clone of the GeocentricSolarEclipticAxes.
 *
 */
//------------------------------------------------------------------------------
GmatBase* GeocentricSolarEclipticAxes::Clone() const
{
   return (new GeocentricSolarEclipticAxes(*this));
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
 * from/to this AxisSystem to/from the MJ2000Eq system
 *
 * @param atEpoch          epoch at which to compute the rotation matrix
 * @param forceComputation force computation even if it's not time to do it
 *                         (default is false)
 */
//------------------------------------------------------------------------------
void GeocentricSolarEclipticAxes::CalculateRotationMatrix(const A1Mjd &atEpoch,
                                                          bool forceComputation) 
{
   #ifdef DEBUG_GSE_CALC
      MessageInterface::ShowMessage("In GSE::CalRotMat, epoch = %12.10f, forceComputtion = %s\n",
            atEpoch.Get(), (forceComputation? "true" : "false"));
      MessageInterface::ShowMessage("   primary   = %s\n", (primary->GetName()).c_str());
      MessageInterface::ShowMessage("   secondary = %s\n", (secondary->GetName()).c_str());
   #endif
   Rvector6 rvSun = secondary->GetMJ2000State(atEpoch) -
                    primary->GetMJ2000State(atEpoch);
   Rvector3 rSun  = rvSun.GetR();
   Rvector3 vSun  = rvSun.GetV();

   Real     rMag  = rSun.GetMagnitude();
   Real     vMag  = vSun.GetMagnitude();

   Rvector3 rUnit = rSun / rMag;
   Rvector3 vUnit = vSun / vMag;
   
   Rvector3 rxv   = Cross(rSun,vSun);
   Real     rxvM  = rxv.GetMagnitude();

   Rvector3 x     = rUnit;
   Rvector3 z     = rxv / rxvM;
   Rvector3 y     = Cross(z,x);

   rotMatrix(0,0) = x(0);
   rotMatrix(0,1) = y(0);
   rotMatrix(0,2) = z(0);
   rotMatrix(1,0) = x(1);
   rotMatrix(1,1) = y(1);
   rotMatrix(1,2) = z(1);
   rotMatrix(2,0) = x(2);
   rotMatrix(2,1) = y(2);
   rotMatrix(2,2) = z(2);

   Rvector3 vR    = vSun / rMag;
   Rvector3 xDot  = vR - x * (x * vR);
   Rvector3 zDot;     // zero vector by default
   Rvector3 yDot  = Cross(z, xDot);

   rotDotMatrix(0,0) = xDot(0);
   rotDotMatrix(0,1) = yDot(0);
   rotDotMatrix(0,2) = zDot(0);
   rotDotMatrix(1,0) = xDot(1);
   rotDotMatrix(1,1) = yDot(1);
   rotDotMatrix(1,2) = zDot(1);
   rotDotMatrix(2,0) = xDot(2);
   rotDotMatrix(2,1) = yDot(2);
   rotDotMatrix(2,2) = zDot(2);

}
