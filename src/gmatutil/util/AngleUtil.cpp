//$Id$
//------------------------------------------------------------------------------
//                             AngleUtil
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Linda Jun
// Created: 2003/09/16
//
/**
 * Implements AngleUtil which provides various angle computation.
 */
//------------------------------------------------------------------------------
#include "utildefs.hpp"
#include "AngleUtil.hpp"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"     // for Mod(), ATan()

using namespace GmatMathConstants;
using namespace GmatMathUtil;

//---------------------------------
// public static functions
//---------------------------------

//------------------------------------------------------------------------------
// static Real AngleUtil::PutAngleInDegRange(Real angleInDeg, Real minAngleInDeg,
//                                           Real maxAngleInDeg)
//------------------------------------------------------------------------------
/**
 * Put angles in given minimum and maximum angle range.
 *
 * @param <angleInDeg> input angle (deg)
 * @param <minAngleInDeg> input minimum angle (deg)
 * @param <maxAngleInDeg> input maximum angle (deg)
 *
 * @return angle in degrees in min/max range
 */
//------------------------------------------------------------------------------
Real AngleUtil::PutAngleInDegRange(Real angleInDeg, Real minAngleInDeg,
                                   Real maxAngleInDeg)
{
   if (angleInDeg >= minAngleInDeg && angleInDeg <= maxAngleInDeg)
      return angleInDeg;
   
   Real angle = Mod(angleInDeg, GmatMathConstants::TWO_PI_DEG);
   
   if (angle < minAngleInDeg)
      angle += GmatMathConstants::TWO_PI_DEG;
   
   else if (angle > maxAngleInDeg)
      angle -= GmatMathConstants::TWO_PI_DEG;
   
   return angle;
}


//------------------------------------------------------------------------------
// static Real AngleUtil::PutAngleInRadRange(Real angleInRad, Real minAngleInRad,
//                                           Real maxAngleInRad)
//------------------------------------------------------------------------------
/**
 * Put angles in given minimum and maximum angle range.
 *
 * @param <angleInRad> input angle (rad)
 * @param <minAngleInRad> input minimum angle (rad)
 * @param <maxAngleInRad> input maximum angle (rad)
 *
 * @return angle in radians in min/max range
 */
//------------------------------------------------------------------------------
Real AngleUtil::PutAngleInRadRange(Real angleInRad, Real minAngleInRad,
                                   Real maxAngleInRad)
{
   if (angleInRad >= minAngleInRad && angleInRad <= maxAngleInRad)
      return angleInRad;
   
   Real angle = Mod(angleInRad, TWO_PI);
   
   if (angle < minAngleInRad)
      angle += TWO_PI;
   
   else if (angle > maxAngleInRad)
      angle -= TWO_PI;
   
   return angle;
}


//------------------------------------------------------------------------------
// static Real ComputePhaseAngleInDeg(Real axis1, Real axis2)
//------------------------------------------------------------------------------
/**
 * Computes phase angle between two numbers.
 *
 * @param <axis1> first axis for phase angle computation
 * @param <axis2> second axis for phase angle computation
 *
 * @return phase angle in degrees between axis1 and axis2
 */
//------------------------------------------------------------------------------
Real AngleUtil::ComputePhaseAngleInDeg(Real axis1, Real axis2)
{
   Real phi;

   phi = ATan(axis1, axis2);

   if(phi < 0.0)
      phi += TWO_PI;

   phi = RadToDeg(phi);
   
   return phi;
}


//------------------------------------------------------------------------------
// static Real ComputePhaseAngleInRad(Real axis1, Real axis2)
//------------------------------------------------------------------------------
/**
 * Computes phase angle between two numbers.
 *
 * @param <axis1> first axis for phase angle computation
 * @param <axis2> second axis for phase angle computation
 *
 * @return phase angle in radians between axis1 and axis2
 */
//------------------------------------------------------------------------------
Real AngleUtil::ComputePhaseAngleInRad(Real axis1, Real axis2)
{
   Real phi;

   phi = ATan(axis1, axis2);

   if(phi < 0.0)
      phi += TWO_PI;
   
   return phi;
}


//------------------------------------------------------------------------------
// Real ComputeAngleInDeg(const Rvector3 &vecA, const Rvector3 &vecB,
//                        Real tol = 0.99)
//------------------------------------------------------------------------------
/**
 * Computes the angle between two 3-element vectors.
 *
 * @param <vecA> First vector
 * @param <vecB> Second vector
 * @param <tol>  Maximum magnitude of dot-product before cross-product is used
 *               to compute angle. (recommened value is 0.99)
 *
 * @return angle between two Rvector3 in degrees.
 */
//------------------------------------------------------------------------------
Real AngleUtil::ComputeAngleInDeg(const Rvector3 &vecA, const Rvector3 &vecB,
                                  Real tol)
{
   Rvector3 uvecA = vecA.GetUnitVector();
   Rvector3 uvecB = vecB.GetUnitVector();
   Real aDotB = uvecA * uvecB;
   Real angRad = 0.0;
   Real angDeg = 0.0;
   
   // if dot-product is less than or equal to tolerance, the angle is arccos of
   // the dot-product, otherwise, the angle is arcsin of the magnitude of the
   // cross-product.
   
   if (Abs(aDotB) <= Abs(tol))
   {
      angRad = ACos(aDotB);
      angDeg = DEG_PER_RAD * angRad;
   }
   else
   {
      // compute cross-product of two vectors
      Rvector3 aCrossB = Cross(uvecA, uvecB);
      Real crossMag = aCrossB.GetMagnitude();
      angRad = ASin(crossMag);

      if (aDotB < 0.0)
      {
         angRad = PI_OVER_TWO - angRad;
         angDeg = DEG_PER_RAD * angRad;
      }
   }
   
   return angDeg;
}
