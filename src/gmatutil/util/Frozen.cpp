//
//------------------------------------------------------------------------------
//                                  Frozen
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author:  Evelyn Hull
// Created: 2011/07/29
//
/**
 * Implements the Frozen base class.
 */
//------------------------------------------------------------------------------

#include "Frozen.hpp"
#include "GmatDefaults.hpp" 
#include "math.h"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"
#include "MessageInterface.hpp"


//------------------------------------------------------------------------------
// Frozen()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the Frozen class
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
Frozen::Frozen()
{
   elements.SMA  = 0.0;
   elements.ALT  = 0.0;
   elements.ECC  = 0.0;
   elements.INC  = 0.0;
   elements.AALT = 0.0;
   elements.PALT = 0.0;

   errormsg      = "";
   isError       = false;
}

//------------------------------------------------------------------------------
// ~Frozen()
//------------------------------------------------------------------------------
/**
 * This method destroys the object of the Frozen class
 * (destructor).
 *
 */
//------------------------------------------------------------------------------
Frozen::~Frozen()
{
}

//------------------------------------------------------------------------------
// void CalculateFrozen(Real ALT, bool altVal, Real INC, bool incVal)
//------------------------------------------------------------------------------
/**
 * Calculates the RepeatSunSync values.
 *
 */
//------------------------------------------------------------------------------
void Frozen::CalculateFrozen(Real ALT, bool altVal, Real INC, bool incVal)
{
   Real SMA, n, a1, a2, a3, a4, eLimit, ecc = -1, e, apogeeAlt, perigeeAlt, error;
   Real J2 = 0.0010826267;
   Real J3 = -0.00000254;
   Real tol = .0000000000000000005;
   isError = false;

   if (!altVal)
   {
	  errormsg = "Mean ALT must be selected";
	  isError = true;
      return;
   }
   else if (!incVal)
   {
	  errormsg = "INC must be selected";
	  isError = true;
      return;
   }

   if (ALT <=0)
   {
      isError = true;
      errormsg = 
	     "Altitude must be greater than or equal to 0";
	  return;
   }
   else if ((INC < 0) || (INC >180))
   {
      isError = true;
      errormsg = 
	     "Inclination must be greater than or equal to 0 and less than 180";
	  return;
   }

   INC = INC*GmatMathConstants::PI/180;
   SMA = ALT + GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2];
   n = sqrt(GmatSolarSystemDefaults::PLANET_MU[2]/pow(SMA,2));

   // Coefficients (a1..a4) of orbit eccentricity cubic equation:
   //    a1*e^3 + a2*e^2 + a3*e + a4 = 0
   a1 = 
      -(3/4.0)*n*
	  pow((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]/SMA),2.0)*
	  J2*sin(INC)*(1-5*pow(cos(INC),2.0));
   a2 = 
      (3/2.0)*n*
	  pow((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]/SMA),3)*
	  J3*(1-(35.0/4)*pow(sin(INC),2)*pow(cos(INC),2));
   a3 = -a1;
   a4 = 
	  (3/2.0)*n*
	  pow((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]/SMA),3)*
	  J3*pow(sin(INC),2)*((5/4.0)*pow(sin(INC),2)-1);

   //Solve orbit eccentricity cubic equation for roots
   eLimit = 0.002; // define absolute limit on e; these are typically < 0.0011
    //typically only 1 of 3 roots is valid (i.e., slightly larger than 0)

   e = 0;
   error = a1*pow(e,3) + a2*pow(e,2) + a3*e + a4;

   while ((GmatMathUtil::Abs(error)>tol) && (e<=eLimit))
   {
	  e+=GmatMathUtil::Abs(error)*.25;
	  error = a1*pow(e,3) + a2*pow(e,2) + a3*e + a4;
   }
   if ((e >= 0) && (e < eLimit))
   {
      apogeeAlt = SMA*(1+e)-GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2];
      perigeeAlt = SMA*(1-e)-GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2];
      ecc = e;
   }
   else
   {
	  apogeeAlt = -1;
      perigeeAlt = -1;
	  ecc = -1;
   }

   if (perigeeAlt <=0)
      errormsg = "Could not find frozen orbit";
   else if (apogeeAlt < perigeeAlt)
      errormsg = "Could not find frozen orbit";
   else if (ecc == -1)
      errormsg = "Could not find frozen orbit";
   else
   {
	  elements.ECC = ecc;
	  elements.SMA = SMA;
	  elements.INC = INC*180/GmatMathConstants::PI;
	  elements.AALT = apogeeAlt;
	  elements.PALT = perigeeAlt;
   }
}

//------------------------------------------------------------------------------
// Real GetSMA()
//------------------------------------------------------------------------------
/**
 * Returns SMA value.
 *
 */
//------------------------------------------------------------------------------
Real Frozen::GetSMA()
{
	return elements.SMA;
}

//------------------------------------------------------------------------------
// Real GetALT()
//------------------------------------------------------------------------------
/**
 * Returns ALT value.
 *
 */
//------------------------------------------------------------------------------
Real Frozen::GetALT()
{
	return elements.ALT;
}

//------------------------------------------------------------------------------
// Real GetECC()
//------------------------------------------------------------------------------
/**
 * Returns ECC value.
 *
 */
//------------------------------------------------------------------------------
Real Frozen::GetECC()
{
	return elements.ECC;
}

//------------------------------------------------------------------------------
// Real GetINC()
//------------------------------------------------------------------------------
/**
 * Returns INC value.
 *
 */
//------------------------------------------------------------------------------
Real Frozen::GetINC()
{
	return elements.INC;
}

//------------------------------------------------------------------------------
// Real GetAALT()
//------------------------------------------------------------------------------
/**
 * Returns AALT value.
 *
 */
//------------------------------------------------------------------------------
Real Frozen::GetAALT()
{
	return elements.AALT;
}

//------------------------------------------------------------------------------
// Real GetPALT()
//------------------------------------------------------------------------------
/**
 * Returns PALT value.
 *
 */
//------------------------------------------------------------------------------
Real Frozen::GetPALT()
{
	return elements.PALT;
}

//------------------------------------------------------------------------------
// bool IsError()
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not there has been an error.
 *
 */
//------------------------------------------------------------------------------
bool Frozen::IsError()
{
	return isError;
}

//------------------------------------------------------------------------------
// std::string GetError()
//------------------------------------------------------------------------------
/**
 * Returns the error message.
 *
 */
//------------------------------------------------------------------------------
std::string Frozen::GetError()
{
	return errormsg;
}
