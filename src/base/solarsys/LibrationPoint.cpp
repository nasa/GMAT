//$Id$
//------------------------------------------------------------------------------
//                                  LibrationPoint
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
// Author: Wendy C. Shoan
// Created: 2005/04/04
//
/**
 * Implementation of the LibrationPoint class.
 */
//------------------------------------------------------------------------------

#include <vector>
#include "gmatdefs.hpp"
#include "CalculatedPoint.hpp"
#include "LibrationPoint.hpp"
#include "Barycenter.hpp"
#include "SolarSystemException.hpp"
#include "CelestialBody.hpp"
#include "RealUtilities.hpp"
#include "Rvector3.hpp"
#include "Rvector6.hpp"
#include "Rmatrix33.hpp"
#include "MessageInterface.hpp"
#include "GmatDefaults.hpp"
#include "ColorTypes.hpp"               // for namespace GmatColor::

#include <iostream>


//#define DEBUG_CHECK_BODIES
//#define DEBUG_LP_OBJECT
//#define DEBUG_LP_RENAME

//---------------------------------
// static data
//---------------------------------
const Real LibrationPoint::CONVERGENCE_TOLERANCE = 1.0E-8;
const Real LibrationPoint::MAX_ITERATIONS        = 2000;
const Real LibrationPoint::ZERO_MASS_TOL         = 1.0E-15;
const Real LibrationPoint::ZERO_MAG_TOL          = 1.0E-12;

const std::string
LibrationPoint::PARAMETER_TEXT[LibrationPointParamCount - CalculatedPointParamCount] =
{
   "Primary",
   "Secondary",
   "Point",
};

const Gmat::ParameterType
LibrationPoint::PARAMETER_TYPE[LibrationPointParamCount - CalculatedPointParamCount] =
{
   Gmat::OBJECT_TYPE,
   Gmat::OBJECT_TYPE,
   Gmat::ENUMERATION_TYPE,
};

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------
//------------------------------------------------------------------------------
//  LibrationPoint(const std::string &itsName)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the LibrationPoint class
 * (default constructor).
 *
 * @param <itsName> parameter indicating the name of the LibrationPoint.
 */
//------------------------------------------------------------------------------
LibrationPoint::LibrationPoint(const std::string &itsName) :
CalculatedPoint("LibrationPoint", itsName),
primaryBodyName     (""),
secondaryBodyName   (""),
whichPoint          (""),
primaryBody         (NULL),
secondaryBody       (NULL)
{
   objectTypes.push_back(Gmat::LIBRATION_POINT);
   objectTypeNames.push_back("LibrationPoint");
   parameterCount = LibrationPointParamCount;
   
   // Set default colors
   SetDefaultColors(GmatColor::GREEN_YELLOW, GmatColor::DARK_GRAY);
}

//------------------------------------------------------------------------------
//  LibrationPoint(const LibrationPoint &lp)
//------------------------------------------------------------------------------
/**
 * This method creates an object of the LibrationPoint class as a copy of the
 * specified LibrationPoint class (copy constructor).
 *
 * @param <lp> LibrationPoint object to copy.
 */
//------------------------------------------------------------------------------
LibrationPoint::LibrationPoint(const LibrationPoint &lp) :
CalculatedPoint          (lp),
primaryBodyName          (lp.primaryBodyName),
secondaryBodyName        (lp.secondaryBodyName),
whichPoint               (lp.whichPoint),
primaryBody              (lp.primaryBody),   // (lp.primaryBody)
secondaryBody            (lp.secondaryBody)    // (lp.secondaryBody)
{
   bodyList = lp.bodyList;
}

//------------------------------------------------------------------------------
//  LibrationPoint& operator= (const LibrationPoint& lp)
//------------------------------------------------------------------------------
/**
 * Assignment operator for the LibrationPoint class.
 *
 * @param <lp> the LibrationPoint object whose data to assign to "this"
 *             calculated point.
 *
 * @return "this" LibrationPoint with data of input LibrationPoint lp.
 */
//------------------------------------------------------------------------------
LibrationPoint& LibrationPoint::operator=(const LibrationPoint &lp)
{
   if (&lp == this)
      return *this;

   CalculatedPoint::operator=(lp);
   primaryBodyName     = lp.primaryBodyName;
   secondaryBodyName   = lp.secondaryBodyName;
   whichPoint          = lp.whichPoint;
   primaryBody         = lp.primaryBody;
   secondaryBody       = lp.secondaryBody;
   return *this;
}

//------------------------------------------------------------------------------
//  ~LibrationPoint()
//------------------------------------------------------------------------------
/**
 * Destructor for the LibrationPoint class.
 */
//------------------------------------------------------------------------------
LibrationPoint::~LibrationPoint()
{
}


//---------------------------------------------------------------------------
//  const Rvector6 GetMJ2000State(const A1Mjd &atTime)
//---------------------------------------------------------------------------
/**
 * Method returning the MJ2000Eq state of the LibrationPoint at the time atTime.
 *
 * @param <atTime> Time for which the state is requested.
 *
 * @return state of the LibrationPoint at time atTime.
 */
//---------------------------------------------------------------------------
const Rvector6 LibrationPoint::GetMJ2000State(const A1Mjd &atTime)
{
   #ifdef DEBUG_GET_STATE
   MessageInterface::ShowMessage
      ("LibrationPoint::GetMJ2000State() '%s' entered, atTime=%f, "
       "primaryBody=<%p> '%s', secondaryBody=<%p> '%s'\n", GetName().c_str(),
       atTime.GetReal(), primaryBody, primaryBody->GetName().c_str(),
       secondaryBody, secondaryBody->GetName().c_str());
   #endif
   
   CheckBodies();
   // Compute position and velocity from primary to secondary
   Rvector6 primaryState   = primaryBody->GetMJ2000State(atTime);
   Rvector6 secondaryState = secondaryBody->GetMJ2000State(atTime);
   
   #ifdef DEBUG_GET_STATE
   MessageInterface::ShowMessage
      ("   primaryState   =\n   %s\n", primaryState.ToString().c_str());
   MessageInterface::ShowMessage
      ("   secondaryState =\n   %s\n", secondaryState.ToString().c_str());
   #endif
   
   Rvector6 pToS = secondaryState - primaryState;
   Rvector3 r    = pToS.GetR();
   Rvector3 v    = pToS.GetV();
   Rvector3 a    = (secondaryBody->GetMJ2000Acceleration(atTime)) -
                   (primaryBody->GetMJ2000Acceleration(atTime));
   
   Real     massPrimary, massSecondary;
   if (primaryBody->IsOfType("CelestialBody"))
      massPrimary = ((CelestialBody*) primaryBody)->GetMass();
   else  // Barycenter
      massPrimary = ((Barycenter*) primaryBody)->GetMass();
   if (secondaryBody->IsOfType("CelestialBody"))
      massSecondary = ((CelestialBody*) secondaryBody)->GetMass();
   else  // Barycenter
      massSecondary = ((Barycenter*) secondaryBody)->GetMass();
   
   // Test that masses are not too small
   if (massPrimary <= LibrationPoint::ZERO_MASS_TOL) 
		 throw SolarSystemException(
		    "The mass of the Primary on LibrationPoint \""
			+ GetName() + "\" is near zero or negative.\n");
   if (massSecondary <= LibrationPoint::ZERO_MASS_TOL) 
		 throw SolarSystemException(
		    "The mass of the Secondary on LibrationPoint \"" 
			+ GetName() + "\" is near zero or negative.\n");
   // Divide by zero is avoided by previous mass tests
   Real muStar = massSecondary / (massPrimary + massSecondary);

   #ifdef DEBUG_GET_STATE
   MessageInterface::ShowMessage
      ("   Mass of the primary is %f\n", massPrimary);
   MessageInterface::ShowMessage
      ("   Mass of the secondary is %f\n", massSecondary);
   #endif
   
   Real gamma  = 0.0;
   Real gamma2 = 0.0, gamma3 = 0.0, gamma4 = 0.0, gamma5 = 0.0, gammaPrev = 0.0;
   Real F = 0.0, Fdot = 0.0;
   if ((whichPoint == "L1") || (whichPoint == "L2") ||
       (whichPoint == "L3"))
   {
      // Determine initial gamma
      if (whichPoint == "L3")  gamma = 1.0;
      else  gamma = GmatMathUtil::Pow((muStar / (3.0 * (1.0 - muStar))),
                                      (1.0 / 3.0));
      

      Integer counter = 0;
      Real diff       = 999.99;
      while (diff > CONVERGENCE_TOLERANCE)
      {
         if (counter > MAX_ITERATIONS)
            throw SolarSystemException(
                  "Libration point \"" + GetName() + "\" gamma not converging.");
         gamma2 = gamma  * gamma;
         gamma3 = gamma2 * gamma;
         gamma4 = gamma3 * gamma;
         gamma5 = gamma4 * gamma;
         if (whichPoint == "L1")
         {
            F    = gamma5 - ((3.0 - muStar) * gamma4) +
                   ((3.0 - 2.0 * muStar) * gamma3) -
                   (muStar * gamma2) + (2.0 * muStar * gamma) - muStar;
            Fdot = (5.0 * gamma4) - (4.0 * (3.0 - muStar) * gamma3) + 
                   (3.0 * (3.0 - 2.0 * muStar) * gamma2) - 
                   (2.0 * muStar * gamma) + (2.0 * muStar);
         }
         else if (whichPoint == "L2")
         {
            F    = gamma5 + ((3.0 - muStar) * gamma4) +
                   ((3.0 - 2.0 * muStar) * gamma3) -
                   (muStar * gamma2) - (2.0 * muStar * gamma) - muStar;
            Fdot = (5.0 * gamma4) + (4.0 * (3.0 - muStar) * gamma3) + 
                   (3.0 * (3.0 - 2.0 * muStar) * gamma2) - (2.0 * muStar * gamma) -
                   (2.0 * muStar);
         }
         else  // whichPoint == "L3"
         {
            F    = gamma5 + ((2.0 + muStar) * gamma4) +
                   ((1.0 + 2.0 * muStar) * gamma3) -
                   ((1.0 - muStar) * gamma2) - (2.0 * (1.0 - muStar) * gamma) -
                   (1.0 - muStar);
            Fdot = (5.0 * gamma4) + (4.0 * (2.0 +  muStar) * gamma3) + 
                   (3.0 * (1.0 +  2.0 * muStar) * gamma2) -
                   (2.0 * (1.0 - muStar) * gamma) - (2.0 * (1.0 - muStar));
         }
         counter++;
         gammaPrev = gamma;
         gamma     = gammaPrev - (F / Fdot);
         diff      = GmatMathUtil::Abs(gamma - gammaPrev);
      }
   }
   Real x = 0.0;
   Real y = 0.0;
   if (whichPoint == "L1") 
   {
      x = 1.0 - gamma;
      y = 0.0;
   }
   else if (whichPoint == "L2")
   {
      x = 1.0 + gamma;
      y = 0.0;
   }
   else if (whichPoint == "L3")
   {
      x = - gamma;
      y = 0.0;
   }
   else if (whichPoint == "L4")
   {
      x = 0.5;
      y = GmatMathUtil::Sqrt(3.0) / 2.0;
   }
   else if (whichPoint == "L5")
   {
      x = 0.5;
      y = - GmatMathUtil::Sqrt(3.0) / 2.0;
   }
   else // ERROR
      throw SolarSystemException
         ("\"" + whichPoint + "\" is illegal value for libration point.");
   
   // Express position and velocity of the libration point in the rotating
   // system with the origin centered on the primary body
   Rvector3 ri(x, y, 0.0);
   Rvector3 vi(x, y, 0.0);
   Real rMag  = r.GetMagnitude();

   if (rMag <= LibrationPoint::ZERO_MAG_TOL) 
		 throw SolarSystemException(
		    "The LibrationPoint \""
			+ GetName() + "\" is undefined because the Primary and Secondary are too close together.\n");

   ri         = rMag * ri;
   Real vMult = (v * r) / rMag;
   vi         = vMult * vi;
   // Determine the rotation matrix and its derivative
   Rvector3 xHat    = r / r.GetMagnitude();  // unit vector
   Rvector3 zHat    = (Cross(r,v)).GetUnitVector();
   Rvector3 yHat    = Cross(zHat, xHat);
   Rvector3 xDotHat = (v / rMag) - (xHat / rMag) * (xHat * v);
   Rvector3 ra      = Cross(r,a);
   Rvector3 rv      = Cross(r,v);
   Real     rvMag   = rv.GetMagnitude();
   Rvector3 zDotHat = ra / rvMag - (zHat / rvMag) * ((ra * zHat));
   Rvector3 yDotHat = Cross(zDotHat, xHat) + Cross(zHat, xDotHat);
   Rmatrix33 R;
   R(0,0) = xHat(0);
   R(0,1) = yHat(0);
   R(0,2) = zHat(0);
   R(1,0) = xHat(1);
   R(1,1) = yHat(1);
   R(1,2) = zHat(1);
   R(2,0) = xHat(2);
   R(2,1) = yHat(2);
   R(2,2) = zHat(2);
   
   Rmatrix33 RDot;
   RDot(0,0) = xDotHat(0);
   RDot(0,1) = yDotHat(0);
   RDot(0,2) = zDotHat(0);
   RDot(1,0) = xDotHat(1);
   RDot(1,1) = yDotHat(1);
   RDot(1,2) = zDotHat(1);
   RDot(2,0) = xDotHat(2);
   RDot(2,1) = yDotHat(2);
   RDot(2,2) = zDotHat(2);
   
   Rvector3 rLi = R * ri;
   Rvector3 vLi = RDot * ri + R * vi;
   
   Rvector6 rvFK5(rLi(0), rLi(1), rLi(2), vLi(0), vLi(1), vLi(2));
   
   // Translate so that the origin is at the j2000Body
   Rvector6 rvResult = rvFK5 + primaryState;
   lastState         = rvResult;
   lastStateTime     = atTime;
   #ifdef DEBUG_GET_STATE
   MessageInterface::ShowMessage
      ("LibrationPoint::GetMJ2000State() returning\n   %s\n",
       rvResult.ToString().c_str());
   #endif
   return rvResult;
}


const Rvector6 LibrationPoint::GetMJ2000State(const GmatTime &atTime)
{
#ifdef DEBUG_GET_STATE
   MessageInterface::ShowMessage
      ("LibrationPoint::GetMJ2000State() '%s' entered, atTime=%f, "
      "primaryBody=<%p> '%s', secondaryBody=<%p> '%s'\n", GetName().c_str(),
      GmatTime(atTime).GetMjd(), primaryBody, primaryBody->GetName().c_str(),
      secondaryBody, secondaryBody->GetName().c_str());
#endif

   CheckBodies();
   // Compute position and velocity from primary to secondary
   Rvector6 primaryState = primaryBody->GetMJ2000State(atTime);
   Rvector6 secondaryState = secondaryBody->GetMJ2000State(atTime);

#ifdef DEBUG_GET_STATE
   MessageInterface::ShowMessage
      ("   primaryState   =\n   %s\n", primaryState.ToString().c_str());
   MessageInterface::ShowMessage
      ("   secondaryState =\n   %s\n", secondaryState.ToString().c_str());
#endif

   Rvector6 pToS = secondaryState - primaryState;
   Rvector3 r = pToS.GetR();
   Rvector3 v = pToS.GetV();
   Rvector3 a = (secondaryBody->GetMJ2000Acceleration(atTime)) -
      (primaryBody->GetMJ2000Acceleration(atTime));

   Real     massPrimary, massSecondary;
   if (primaryBody->IsOfType("CelestialBody"))
      massPrimary = ((CelestialBody*)primaryBody)->GetMass();
   else  // Barycenter
      massPrimary = ((Barycenter*)primaryBody)->GetMass();
   if (secondaryBody->IsOfType("CelestialBody"))
      massSecondary = ((CelestialBody*)secondaryBody)->GetMass();
   else  // Barycenter
      massSecondary = ((Barycenter*)secondaryBody)->GetMass();

   // Test that masses are not too small
   if (massPrimary <= LibrationPoint::ZERO_MASS_TOL)
      throw SolarSystemException(
      "The mass of the Primary on LibrationPoint \""
      + GetName() + "\" is near zero or negative.\n");
   if (massSecondary <= LibrationPoint::ZERO_MASS_TOL)
      throw SolarSystemException(
      "The mass of the Secondary on LibrationPoint \""
      + GetName() + "\" is near zero or negative.\n");
   // Divide by zero is avoided by previous mass tests
   Real muStar = massSecondary / (massPrimary + massSecondary);

#ifdef DEBUG_GET_STATE
   MessageInterface::ShowMessage
      ("   Mass of the primary is %f\n", massPrimary);
   MessageInterface::ShowMessage
      ("   Mass of the secondary is %f\n", massSecondary);
#endif

   Real gamma = 0.0;
   Real gamma2 = 0.0, gamma3 = 0.0, gamma4 = 0.0, gamma5 = 0.0, gammaPrev = 0.0;
   Real F = 0.0, Fdot = 0.0;
   if ((whichPoint == "L1") || (whichPoint == "L2") ||
      (whichPoint == "L3"))
   {
      // Determine initial gamma
      if (whichPoint == "L3")  gamma = 1.0;
      else  gamma = GmatMathUtil::Pow((muStar / (3.0 * (1.0 - muStar))),
         (1.0 / 3.0));


      Integer counter = 0;
      Real diff = 999.99;
      while (diff > CONVERGENCE_TOLERANCE)
      {
         if (counter > MAX_ITERATIONS)
            throw SolarSystemException(
            "Libration point \"" + GetName() + "\" gamma not converging.");
         gamma2 = gamma  * gamma;
         gamma3 = gamma2 * gamma;
         gamma4 = gamma3 * gamma;
         gamma5 = gamma4 * gamma;
         if (whichPoint == "L1")
         {
            F = gamma5 - ((3.0 - muStar) * gamma4) +
               ((3.0 - 2.0 * muStar) * gamma3) -
               (muStar * gamma2) + (2.0 * muStar * gamma) - muStar;
            Fdot = (5.0 * gamma4) - (4.0 * (3.0 - muStar) * gamma3) +
               (3.0 * (3.0 - 2.0 * muStar) * gamma2) -
               (2.0 * muStar * gamma) + (2.0 * muStar);
         }
         else if (whichPoint == "L2")
         {
            F = gamma5 + ((3.0 - muStar) * gamma4) +
               ((3.0 - 2.0 * muStar) * gamma3) -
               (muStar * gamma2) - (2.0 * muStar * gamma) - muStar;
            Fdot = (5.0 * gamma4) + (4.0 * (3.0 - muStar) * gamma3) +
               (3.0 * (3.0 - 2.0 * muStar) * gamma2) - (2.0 * muStar * gamma) -
               (2.0 * muStar);
         }
         else  // whichPoint == "L3"
         {
            F = gamma5 + ((2.0 + muStar) * gamma4) +
               ((1.0 + 2.0 * muStar) * gamma3) -
               ((1.0 - muStar) * gamma2) - (2.0 * (1.0 - muStar) * gamma) -
               (1.0 - muStar);
            Fdot = (5.0 * gamma4) + (4.0 * (2.0 + muStar) * gamma3) +
               (3.0 * (1.0 + 2.0 * muStar) * gamma2) -
               (2.0 * (1.0 - muStar) * gamma) - (2.0 * (1.0 - muStar));
         }
         counter++;
         gammaPrev = gamma;
         gamma = gammaPrev - (F / Fdot);
         diff = GmatMathUtil::Abs(gamma - gammaPrev);
      }
   }
   Real x = 0.0;
   Real y = 0.0;
   if (whichPoint == "L1")
   {
      x = 1.0 - gamma;
      y = 0.0;
   }
   else if (whichPoint == "L2")
   {
      x = 1.0 + gamma;
      y = 0.0;
   }
   else if (whichPoint == "L3")
   {
      x = -gamma;
      y = 0.0;
   }
   else if (whichPoint == "L4")
   {
      x = 0.5;
      y = GmatMathUtil::Sqrt(3.0) / 2.0;
   }
   else if (whichPoint == "L5")
   {
      x = 0.5;
      y = -GmatMathUtil::Sqrt(3.0) / 2.0;
   }
   else // ERROR
      throw SolarSystemException
      ("\"" + whichPoint + "\" is illegal value for libration point.");

   // Express position and velocity of the libration point in the rotating
   // system with the origin centered on the primary body
   Rvector3 ri(x, y, 0.0);
   Rvector3 vi(x, y, 0.0);
   Real rMag = r.GetMagnitude();

   if (rMag <= LibrationPoint::ZERO_MAG_TOL)
      throw SolarSystemException(
      "The LibrationPoint \""
      + GetName() + "\" is undefined because the Primary and Secondary are too close together.\n");

   ri = rMag * ri;
   Real vMult = (v * r) / rMag;
   vi = vMult * vi;
   // Determine the rotation matrix and its derivative
   Rvector3 xHat = r / r.GetMagnitude();  // unit vector
   Rvector3 zHat = (Cross(r, v)).GetUnitVector();
   Rvector3 yHat = Cross(zHat, xHat);
   Rvector3 xDotHat = (v / rMag) - (xHat / rMag) * (xHat * v);
   Rvector3 ra = Cross(r, a);
   Rvector3 rv = Cross(r, v);
   Real     rvMag = rv.GetMagnitude();
   Rvector3 zDotHat = ra / rvMag - (zHat / rvMag) * ((ra * zHat));
   Rvector3 yDotHat = Cross(zDotHat, xHat) + Cross(zHat, xDotHat);
   Rmatrix33 R;
   R(0, 0) = xHat(0);
   R(0, 1) = yHat(0);
   R(0, 2) = zHat(0);
   R(1, 0) = xHat(1);
   R(1, 1) = yHat(1);
   R(1, 2) = zHat(1);
   R(2, 0) = xHat(2);
   R(2, 1) = yHat(2);
   R(2, 2) = zHat(2);

   Rmatrix33 RDot;
   RDot(0, 0) = xDotHat(0);
   RDot(0, 1) = yDotHat(0);
   RDot(0, 2) = zDotHat(0);
   RDot(1, 0) = xDotHat(1);
   RDot(1, 1) = yDotHat(1);
   RDot(1, 2) = zDotHat(1);
   RDot(2, 0) = xDotHat(2);
   RDot(2, 1) = yDotHat(2);
   RDot(2, 2) = zDotHat(2);

   Rvector3 rLi = R * ri;
   Rvector3 vLi = RDot * ri + R * vi;

   Rvector6 rvFK5(rLi(0), rLi(1), rLi(2), vLi(0), vLi(1), vLi(2));

   // Translate so that the origin is at the j2000Body
   Rvector6 rvResult = rvFK5 + primaryState;
   lastState = rvResult;
   lastStateTimeGT = atTime;
   lastStateTime   = GmatTime(atTime).GetMjd();

#ifdef DEBUG_GET_STATE
   MessageInterface::ShowMessage
      ("LibrationPoint::GetMJ2000State() returning\n   %s\n",
      rvResult.ToString().c_str());
#endif
   return rvResult;
}


//---------------------------------------------------------------------------
//  const Rvector3 GetMJ2000Position(const A1Mjd &atTime)
//---------------------------------------------------------------------------
/**
 * Method returning the MJ2000Eq position of the LibrationPoint at the time atTime.
 *
 * @param <atTime> Time for which the position is requested.
 *
 * @return position of the LibrationPoint at time atTime.
 */
//---------------------------------------------------------------------------
const Rvector3 LibrationPoint::GetMJ2000Position(const A1Mjd &atTime)
{
   Rvector6 tmp = GetMJ2000State(atTime);
   return (tmp.GetR());
}


const Rvector3 LibrationPoint::GetMJ2000Position(const GmatTime &atTime)
{
   Rvector6 tmp = GetMJ2000State(atTime);
   return (tmp.GetR());
}


//---------------------------------------------------------------------------
//  const Rvector3 GetMJ2000Velocity(const A1Mjd &atTime)
//---------------------------------------------------------------------------
/**
 * Method returning the MJ2000Eq velocity of the LibrationPoint at the time atTime.
 *
 * @param <atTime> Time for which the velocity is requested.
 *
 * @return velocity of the LibrationPoint at time atTime.
 */
//---------------------------------------------------------------------------
const Rvector3 LibrationPoint::GetMJ2000Velocity(const A1Mjd &atTime)
{
   Rvector6 tmp = GetMJ2000State(atTime);
   return (tmp.GetV());
}


const Rvector3 LibrationPoint::GetMJ2000Velocity(const GmatTime &atTime)
{
   Rvector6 tmp = GetMJ2000State(atTime);
   return (tmp.GetV());
}


//---------------------------------------------------------------------------
//  StringArray GetBuiltInNames()
//---------------------------------------------------------------------------
/**
 * Returns the name(s) of the built-in space point.
 *
 * @return  name of the space point for the built-in libration point
 */
//---------------------------------------------------------------------------
StringArray LibrationPoint::GetBuiltInNames()
{
   StringArray spNames;
   // None at this time
   return spNames;
}

//------------------------------------------------------------------------------
//  std::string  GetParameterText(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter text, given the input parameter ID.
 *
 * @param <id> Id for the requested parameter text.
 *
 * @return parameter text for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string LibrationPoint::GetParameterText(const Integer id) const
{
   if (id >= CalculatedPointParamCount && id < LibrationPointParamCount)
      return PARAMETER_TEXT[id - CalculatedPointParamCount];
   return CalculatedPoint::GetParameterText(id);
}

//------------------------------------------------------------------------------
//  Integer  GetParameterID(const std::string &str) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter ID, given the input parameter string.
 *
 * @param <str> string for the requested parameter.
 *
 * @return ID for the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Integer     LibrationPoint::GetParameterID(const std::string &str) const
{
   for (Integer i = CalculatedPointParamCount; i < LibrationPointParamCount; i++)
   {
      if (str == PARAMETER_TEXT[i - CalculatedPointParamCount])
         return i;
   }
   
   return CalculatedPoint::GetParameterID(str);
}

//------------------------------------------------------------------------------
//  Gmat::ParameterType  GetParameterType(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
Gmat::ParameterType LibrationPoint::GetParameterType(const Integer id) const
{
   if (id >= CalculatedPointParamCount && id < LibrationPointParamCount)
      return PARAMETER_TYPE[id - CalculatedPointParamCount];
      
   return CalculatedPoint::GetParameterType(id);
}

//------------------------------------------------------------------------------
//  std::string  GetParameterTypeString(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the parameter type string, given the input parameter ID.
 *
 * @param <id> ID for the requested parameter.
 *
 * @return parameter type string of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string LibrationPoint::GetParameterTypeString(const Integer id) const
{
   return CalculatedPoint::PARAM_TYPE_STRING[GetParameterType(id)];
}

//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const Integer id) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter ID.
 *
 * @param <id>    ID for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string LibrationPoint::GetStringParameter(const Integer id) const
{
   if (id == PRIMARY_BODY_NAME)             
   {
      return primaryBodyName;
   }
   if (id == SECONDARY_BODY_NAME)             
   {
      return secondaryBodyName;
   }
   if (id == WHICH_POINT)             
   {
      return whichPoint;
   }
   
   return CalculatedPoint::GetStringParameter(id);
}


//------------------------------------------------------------------------------
//  std::string  GetStringParameter(const std::string &label) const
//------------------------------------------------------------------------------
/**
 * This method returns the string parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 *
 * @return  string value of the requested parameter.
 *
 */
//------------------------------------------------------------------------------
std::string LibrationPoint::GetStringParameter(const std::string &label) const
{
   return GetStringParameter(GetParameterID(label));
}


//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const char *value)
//------------------------------------------------------------------------------
/**
 * @see SetStringParameter(const Integer id, const std::string &value)
 */
//------------------------------------------------------------------------------
bool LibrationPoint::SetStringParameter(const Integer id, 
                                        const char *value)
{
   return SetStringParameter(id, std::string(value));
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const Integer id, const std::string &value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID.
 *
 * @param <id> ID for the requested parameter.
 * @param <value> string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool LibrationPoint::SetStringParameter(const Integer id, 
                                        const std::string &value)
{
   if (id == BODY_NAMES)
   {
      std::string errmsg = "The field \"";
      errmsg += GetParameterText(id) + "\" on LibrationPoint \"";
      errmsg += instanceName + "\" is not allowed.\n";
      errmsg += "Use \"" + GetParameterText(PRIMARY_BODY_NAME) + "\" and \"";
      errmsg += GetParameterText(SECONDARY_BODY_NAME);
      errmsg += "\" to set bodies on a LibrationPoint.\n";
      throw SolarSystemException(errmsg);
   }
   if (id == PRIMARY_BODY_NAME)             
   {
      // since we don't know the order of setting, we cannot do the checking
      // to see if primary and secondary bodies are the same
      primaryBodyName = value;
      ValidateBodyName(value, false);
      return true;
   }
   if (id == SECONDARY_BODY_NAME)             
   {
      // since we don't know the order of setting, we cannot do the checking
      // to see if primary and secondary bodies are the same
      secondaryBodyName = value;
      ValidateBodyName(value, false);
      return true;
   }
   if (id == WHICH_POINT)             
   {
      if ((value != "L1") && (value != "L2") && (value != "L3") &&
          (value != "L4") && (value != "L5"))
         throw SolarSystemException(
            "The value of \"" + value + "\" for field \"Libration\""
            " on object \"" + instanceName + "\" is not an allowed value.\n"
            "The allowed values are: [ L1, L2, L3, L4, L5 ]. ");
      whichPoint = value;
      return true;
   }
   
   return CalculatedPoint::SetStringParameter(id, value);
}

//------------------------------------------------------------------------------
//  std::string  SetStringParameter(const std::string &label, 
//                                  const std::string value)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter label.
 *
 * @param <label> label for the requested parameter.
 * @param <value> string value for the requested parameter.
 *
 * @return  success flag.
 *
 */
//------------------------------------------------------------------------------
bool LibrationPoint::SetStringParameter(const std::string &label, 
                                        const std::string &value)
{
   return SetStringParameter(GetParameterID(label), value);
}

//------------------------------------------------------------------------------
//  bool  SetStringParameter(const Integer id, const std::string value,
//                           const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter ID, and the index.
 *
 * @param <id>    ID for the requested parameter.
 * @param <value> string value for the requested parameter.
 * @param <index> index into the array of strings.
 *
 * @return  success flag.
 *
 * @note ADDED TO SATISFY DUFUS COMPILER
 *
 */
//------------------------------------------------------------------------------
bool LibrationPoint::SetStringParameter(const Integer id,
                                        const std::string &value,
                                        const Integer index)
{
   if (id == BODY_NAMES)
   {
      std::string errmsg = "The field \"";
      errmsg += GetParameterText(id) + "\" on LibrationPoint \"";
      errmsg += instanceName + "\" is not allowed.\n";
      errmsg += "Use \"" + GetParameterText(PRIMARY_BODY_NAME) + "\" and \"";
      errmsg += GetParameterText(SECONDARY_BODY_NAME);
      errmsg += "\" to set bodies on a LibrationPoint.\n";
      throw SolarSystemException(errmsg);
   }
   return CalculatedPoint::SetStringParameter(id, value, index);
}

//------------------------------------------------------------------------------
//  bool  SetStringParameter(const std::string &label, const std::string value.
//                           const Integer index)
//------------------------------------------------------------------------------
/**
 * This method sets the string parameter value, given the input
 * parameter label, and the index.
 *
 * @param <label> label for the requested parameter.
 * @param <value> string value for the requested parameter.
 * @param <index> index into the array of strings.
 *
 * @return  success flag.
 *
 * @note ADDED TO SATISFY DUFUS COMPILER
 *
 */
//------------------------------------------------------------------------------
bool LibrationPoint::SetStringParameter(const std::string &label,
                                        const std::string &value,
                                        const Integer index)
{
   return SetStringParameter(GetParameterID(label),value,index);
}


//------------------------------------------------------------------------------
// const ObjectTypeArray& GetRefObjectTypeArray()
//------------------------------------------------------------------------------
/**
 * Retrieves the list of ref object types used by this class.
 *
 * @return the list of object types.
 * 
 */
//------------------------------------------------------------------------------
const ObjectTypeArray& LibrationPoint::GetRefObjectTypeArray()
{
   refObjectTypes.clear();
   refObjectTypes.push_back(Gmat::SPACE_POINT);
   return refObjectTypes;
}

//------------------------------------------------------------------------------
//  const StringArray& GetRefObjectNameArray(const UnsignedInt type)
//------------------------------------------------------------------------------
/**
 * Returns the names of the reference object. 
 *
 * @param <type> reference object type.  Gmat::UnknownObject returns all of the
 *               ref objects.
 *
 * @return The names of the reference object.
 */
//------------------------------------------------------------------------------
const StringArray& LibrationPoint::GetRefObjectNameArray(const UnsignedInt type)
{
   if (type == Gmat::UNKNOWN_OBJECT || type == Gmat::SPACE_POINT)
   {
      static StringArray refs;
      refs.clear();
      refs.push_back(primaryBodyName);
      refs.push_back(secondaryBodyName);
      return refs;
   }
   
   // Not handled here -- invoke the next higher GetRefObjectNameArray call
   return CalculatedPoint::GetRefObjectNameArray(type);
}


//------------------------------------------------------------------------------
// bool SetRefObject(GmatBase *obj, const UnsignedInt type,
//                   const std::string &name)
//------------------------------------------------------------------------------
/**
 * Sets the reference object.
 *
 * @param <obj>   reference object pointer.
 * @param <type>  type of the reference object.
 * @param <name>  name of the reference object.
 *
 * @return success of the operation.
 */
//------------------------------------------------------------------------------
bool LibrationPoint::SetRefObject(GmatBase *obj, 
                                  const UnsignedInt type,
                                  const std::string &name)
{
   if (obj == NULL)
      return false;
   
   #ifdef DEBUG_LP_OBJECT
   MessageInterface::ShowMessage
      ("LibrationPoint::SetRefObject() this=<%p> '%s', obj=<%p><%s> entered\n",
       this, GetName().c_str(), obj, obj->GetName().c_str());
   MessageInterface::ShowMessage
      ("   primaryBodyName='%s', primaryBody=<%p>\n   secondaryBodyName='%s', "
       "secondaryBody=<%p>\n", primaryBodyName.c_str(), primaryBody,
       secondaryBodyName.c_str(), secondaryBody);
   #endif
   
   if ((obj->IsOfType(Gmat::SPACE_POINT)) || (obj->IsOfType(Gmat::CALCULATED_POINT)))
   {
      if (name == primaryBodyName)
         primaryBody = (SpacePoint*)obj;
      else if (name == secondaryBodyName)
         secondaryBody = (SpacePoint*)obj;
   }
   #ifdef DEBUG_LP_OBJECT
   MessageInterface::ShowMessage
      ("   end of SetRefObject() this=<%p> '%s', obj=<%p><%s> entered\n",
       this, GetName().c_str(), obj, obj->GetName().c_str());
   MessageInterface::ShowMessage
      ("   primaryBodyName='%s', primaryBody=<%p>\n   secondaryBodyName='%s', "
       "secondaryBody=<%p>\n", primaryBodyName.c_str(), primaryBody,
       secondaryBodyName.c_str(), secondaryBody);
   #endif

   // Check that ref objects are of valid type
   if ((obj->GetName() == GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME) ||
	   ((!obj->IsOfType("CelestialBody") ) && (!obj->IsOfType("Barycenter") )))
		 throw SolarSystemException(
		    "The value of \"" + obj->GetName() + "\" for field \"Primary\""
            " or \"Secondary\" "
			"on LibrationPoint \"" + GetName() + "\" is not an allowed value.\n"
			"The allowed values are: [CelestialBody or Barycenter (except SSB)]. ");

   // Call parent class to add objects to bodyList
   return CalculatedPoint::SetRefObject(obj, type, name);
}


//------------------------------------------------------------------------------
//  bool RenameRefObject(const UnsignedInt type,
//                       const std::string &oldName, const std::string &newName)
//------------------------------------------------------------------------------
/**
 * Interface used to support user actions.
 *
 * @param <type>    reference object type.
 * @param <oldName> object name to be renamed.
 * @param <newName> new object name.
 *
 * @return true if object name changed, false if not.
 */
//------------------------------------------------------------------------------
bool LibrationPoint::RenameRefObject(const UnsignedInt type,
                                      const std::string &oldName,
                                      const std::string &newName)
{
   #ifdef DEBUG_LP_RENAME
      MessageInterface::ShowMessage("Entering LP::Rename with type = %d, oldName = %s, newName = %s\n",
            (Integer) type, oldName.c_str(), newName.c_str());
   #endif
   if ((type == Gmat::SPACE_POINT) || (type == Gmat::CALCULATED_POINT))
   {
      if (primaryBodyName == oldName)
         primaryBodyName = newName;
      if (secondaryBodyName == oldName)
         secondaryBodyName = newName;
   }

   return CalculatedPoint::RenameRefObject(type, oldName, newName);
}



//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the LibrationPoint.
 *
 * @return clone of the LibrationPoint.
 *
 */
//------------------------------------------------------------------------------
GmatBase* LibrationPoint::Clone() const
{
   return (new LibrationPoint(*this));
}


//---------------------------------------------------------------------------
// void Copy(const GmatBase* orig)
//---------------------------------------------------------------------------
/**
 * Sets this object to match another one.
 * 
 * @param orig The original that is being copied.
 */
//---------------------------------------------------------------------------
void LibrationPoint::Copy(const GmatBase* orig)
{
   // We don't want to copy instanceName
   std::string name = instanceName;
   operator=(*((LibrationPoint *)(orig)));
   instanceName = name;
}


//------------------------------------------------------------------------------
// private methods
//------------------------------------------------------------------------------


//------------------------------------------------------------------------------
//  void CheckBodies()
//------------------------------------------------------------------------------
/**
 * Method for the LibrationPoint class that checks to make sure the bodyList has
 * been defined appropriately (i.e. all CelestialBody objects).
 *
 */
//------------------------------------------------------------------------------
void LibrationPoint::CheckBodies()
{
   bool foundPrimary   = false;
   bool foundSecondary = false;
   
   #ifdef DEBUG_CHECK_BODIES
   MessageInterface::ShowMessage
      ("LibrationPoint::CheckBodies() this=<%p> '%s' has %d body names, and %d body"
       " pointers\n", this, GetName().c_str(), bodyNames.size(), bodyList.size());
   #endif
   for (unsigned int i = 0; i < bodyList.size() ; i++)
   {
      #ifdef DEBUG_CHECK_BODIES
      MessageInterface::ShowMessage
         ("   bodyList[%d] = %s\n", i,
          (bodyList[i] == NULL ? "NULL" : bodyList[i]->GetName().c_str()));
      #endif
      if (bodyList.at(i)->GetName() == GmatSolarSystemDefaults::SOLAR_SYSTEM_BARYCENTER_NAME)
		 throw SolarSystemException(
             "Cannot Use SolarSystemBarycenter in a LibrationPoint");
      if (((bodyList.at(i))->GetType() != Gmat::CELESTIAL_BODY) &&
          ((bodyList.at(i))->GetTypeName() != "Barycenter"))
         throw SolarSystemException(
             "Bodies for LibrationPoint must be CelestialBodys or Barycenters");
      if ((bodyList.at(i))->GetName() == primaryBodyName) 
      {
         foundPrimary = true;
         primaryBody  = bodyList.at(i);
      }
      if ((bodyList.at(i))->GetName() == secondaryBodyName) 
      {
         foundSecondary = true;
         secondaryBody  = bodyList.at(i);
      }
   }
   if (!foundPrimary)
      throw SolarSystemException(
            "Primary body \"" + primaryBodyName + "\" not found for LibrationPoint " +
            "\"" + GetName() + "\"");
   if (!foundSecondary)
      throw SolarSystemException(
            "Secondary body \"" + secondaryBodyName + "\" not found for LibrationPoint " +
            "\"" + GetName() + "\"");
   if (primaryBody == secondaryBody)
      throw SolarSystemException(
            "Primary body \"" + primaryBodyName + "\" and Secondary body \"" +
            secondaryBodyName + "\" cannot be the same for LibrationPoint " +
            "\"" + GetName() + "\"");
}


