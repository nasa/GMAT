//
//------------------------------------------------------------------------------
//                                  RepeatSunSync
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
// Created: 2011/07/28
//
/**
 * Implements the Repeat Sun Sync base class.
 */
//------------------------------------------------------------------------------

#include "RepeatSunSync.hpp"
#include "GmatDefaults.hpp" 
#include "math.h"
#include "GmatConstants.hpp"
#include "RealUtilities.hpp"

//------------------------------------------------------------------------------
// RepeatSunSync()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the RepeatSunSync class
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
RepeatSunSync::RepeatSunSync()
{
   elements.SMA          = 0.0;
   elements.ECC          = 0.0;
   elements.INC          = 0.0;
   elements.daysToRepeat = 0.0;
   elements.revsToRepeat = 0.0;
   elements.revsPerDay   = 0.0;

   errormsg     = "";
   isError      = false;
}

//------------------------------------------------------------------------------
// ~RepeatSunSync()
//------------------------------------------------------------------------------
/**
 * This method destroys the object of the RepeatSunSync class
 * (destructor).
 *
 */
//------------------------------------------------------------------------------
RepeatSunSync::~RepeatSunSync()
{
}

//------------------------------------------------------------------------------
// Real GetSMA()
//------------------------------------------------------------------------------
/**
 * Returns SMA value.
 *
 */
//------------------------------------------------------------------------------
Real RepeatSunSync::GetSMA()
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
Real RepeatSunSync::GetALT()
{
	return (elements.SMA-GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]);
}

//------------------------------------------------------------------------------
// Real GetECC()
//------------------------------------------------------------------------------
/**
 * Returns ECC value.
 *
 */
//------------------------------------------------------------------------------
Real RepeatSunSync::GetECC()
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
Real RepeatSunSync::GetINC()
{
	return elements.INC;
}

//------------------------------------------------------------------------------
// Real GetROA()
//------------------------------------------------------------------------------
/**
 * Returns ROA value.
 *
 */
//------------------------------------------------------------------------------
Real RepeatSunSync::GetROA()
{
	return elements.SMA*(1+elements.ECC);
}

//------------------------------------------------------------------------------
// Real GetROP()
//------------------------------------------------------------------------------
/**
 * Returns ROP value.
 *
 */
//------------------------------------------------------------------------------
Real RepeatSunSync::GetROP()
{
	return elements.SMA*(1-elements.ECC);
}

//------------------------------------------------------------------------------
// Real GetP()
//------------------------------------------------------------------------------
/**
 * Returns P value.
 *
 */
//------------------------------------------------------------------------------
Real RepeatSunSync::GetP()
{
	return elements.SMA*(1-pow(elements.ECC,2));
}

//------------------------------------------------------------------------------
// bool IsError()
//------------------------------------------------------------------------------
/**
 * Returns a flag indicating whether or not there has been an error.
 *
 */
//------------------------------------------------------------------------------
bool RepeatSunSync::IsError()
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
std::string RepeatSunSync::GetError()
{
	return errormsg;
}

//------------------------------------------------------------------------------
// void CalculateRepeatSunSync(bool eccVal, Real ECC, bool dtrVal,
//                             Real daysToRepeat, bool rtrVal,
//                             Real revsToRepeat, bool rpdVal,
//                             Real revsPerDay)
//------------------------------------------------------------------------------
/**
 * Calculates the RepeatSunSync values.
 *
 */
//------------------------------------------------------------------------------
void RepeatSunSync::CalculateRepeatSunSync(bool eccVal, Real ECC, bool dtrVal,
      Real daysToRepeat, bool rtrVal, Real revsToRepeat, bool rpdVal, 
	  Real revsPerDay)
{
   Real n, inc, a, sun, diff, p, raanDot, omegaDot, MDot, newA, rp, newE;
   Real incCheck, newInc;
   int count;
   Real J2 = 0.0010826267;
   Real omegaEarth  = 0.000072921158533;
   Real omegaSunSync = (2*GmatMathConstants::PI/365.2422)/86400;
   Real lambdaDot = 0.985*GmatMathConstants::PI/180;

   Real sunTol = .01;
   Real smaTol = .000000001;

   //fixes warnings
   newInc = 0.0;
   newE = 0.0;
   newA = 0.0;

   isError = false;

   if (!eccVal)
   {
	  errormsg = "ECC must be selected";
	  isError = true;
      return;
   }

   if (rtrVal && rpdVal)
   {
      if (revsToRepeat<1)
	  {
            errormsg = 
			   "Please choose revs to repeat greater than or equal to 1";
			isError = true;
            return;
	  }
      else if (revsPerDay<1)
	  {
            errormsg = 
			   "Please choose revs per day greater than or equal to 1";
            isError = true;
            return;
	  }
      else
	  {
         daysToRepeat = revsToRepeat/revsPerDay;
	  }
   }
   else if (dtrVal && rpdVal)
   {
      if (daysToRepeat<1)
	  {
         errormsg = 
		    "Please choose days to repeat greater than or equal to 1";
         isError = true;
         return;
	  }
      else if (revsPerDay<1)
	  {
         errormsg = 
		    "Please choose revs per day greater than or equal to 1";
         isError = true;
         return;
	  }
      else
	  {
         revsToRepeat = revsPerDay*daysToRepeat;
	  }
   }
   else if (rtrVal && dtrVal)
   {
      if (revsToRepeat<1)
	  {
         errormsg = 
		    "Please choose revs to repeat greater than or equal to 1";
         isError = true;
         return;
	  }
      else if (daysToRepeat<1)
	  {
         errormsg = 
		    "Please choose days to repeat greater than or equal to 1";
         isError = true;
         return;
	  }
      else
	  {
         revsPerDay = revsToRepeat/daysToRepeat;
	  }
   }
   else
   {
      errormsg = 
	     "Please select two of days to repeat, revs to repeat, and revs per day";
      isError = true;
      return;
   }

   if ((ECC<0) || (ECC>=1))
   {
      errormsg = "ECC out of range, 0<=ECC<1";
      isError = true;
      return;
   }

   n = revsPerDay*omegaEarth;
   count = 0;
   sun = 1;
   diff = 1;
   inc = 98*GmatMathConstants::PI/180;
   a = 7000;
   errormsg = "";

   while ((GmatMathUtil::Abs(sun) > sunTol) && (diff> smaTol) && (count <= 100))
   {
      p = a*(1 - pow(ECC,2));
      raanDot = 
	     (-3*n*J2/2)*
		 pow((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]/p),2)
		 *cos(inc);
      omegaDot = 
		 (3*n*J2/4)*
		 pow((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]/p),2)*
		 (4-5*pow(sin(inc),2));
      MDot = 
		 (3*n*J2/4)*
		 pow((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]/p),2)*
		 sqrt(1-pow(ECC,2))*(2-3*pow(sin(inc),2));
      sun = 
		 cos(inc) + 
		 ((2*lambdaDot*pow(a,(3.0/2))*pow((1-pow(ECC,2)),2))/
		 (3*J2*pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*
		 sqrt(GmatSolarSystemDefaults::PLANET_MU[2])));
      n = revsPerDay*(omegaEarth - raanDot) - (MDot + omegaDot);
      newA = pow((GmatSolarSystemDefaults::PLANET_MU[2]/pow(n,2)),(1/3.0));
      diff = GmatMathUtil::Abs(newA - a);
      rp = newA*(1-ECC);
      newE = (newA - rp)/newA;
      incCheck = 
	     (2*omegaSunSync*pow((1-pow(ECC,2)),2)*pow(a,(7.0/2)))/
		 (-3*sqrt(GmatSolarSystemDefaults::PLANET_MU[2])*
		 pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*J2);
      if (GmatMathUtil::Abs(incCheck) > 1)
         newInc = inc;
      else
         newInc = acos(incCheck);
      if (newA <= GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2])
	  {
         errormsg = "Could not find orbit";
		 isError = true;
         return;
	  }
      else
	  {
         a = newA;
	  }
      if ((newE >= 1) || (newE < 0))
	  {
         errormsg = "Could not find orbit";
		 isError = true;
         return;
	  }
      else
	  {
         ECC = newE;
	  }
      if ((newInc*180/GmatMathConstants::PI >= 110) || 
		 (newInc*180/GmatMathConstants::PI < 90))
	  {
         errormsg = "Could not find orbit";
		 isError = true;
         newInc = newInc*180/GmatMathConstants::PI;
         return;
	  }
      else
	  {
         inc  = newInc;
	  }
      count++;
   }
    
   elements.INC = newInc*180/GmatMathConstants::PI;
   elements.SMA = newA;
   elements.ECC = newE;
   elements.daysToRepeat = daysToRepeat;
   elements.revsToRepeat = revsToRepeat;
   elements.revsPerDay = revsPerDay;
    
   if (count > 100)
   {
      errormsg = "Could not find orbit in 100 iterations";
      isError = true;
   }
}
