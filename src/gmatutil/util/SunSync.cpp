//
//------------------------------------------------------------------------------
//                                  SunSync
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
// Created: 2011/07/20
//
/**
 * Implements the SunSync base class.
 */
//------------------------------------------------------------------------------

#include "SunSync.hpp"
#include "GmatDefaults.hpp" 
#include <math.h>
#include "GmatConstants.hpp"
#include "MessageInterface.hpp"
#include "RealUtilities.hpp"


//------------------------------------------------------------------------------
// SunSync()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SunSync class
 * (default constructor).
 *
 */
//------------------------------------------------------------------------------
SunSync::SunSync()
{
   elements.SMA = 0.0;
   elements.ALT = 0.0;
   elements.ECC = 0.0;
   elements.INC = 0.0;
   elements.ROP = 0.0;
   elements.ROA = 0.0;
   elements.P   = 0.0;

   errormsg     = "";
   isError      = false;
}

//------------------------------------------------------------------------------
// SunSync()
//------------------------------------------------------------------------------
/**
 * This method creates an object of the SunSync class
 * constructor).
 *
 * @param  a, alt, e, i, rop, roa, p    initial values for the elements
 *
 */
//------------------------------------------------------------------------------
SunSync::SunSync(Real a, Real alt, Real e, Real i, Real rop, Real roa, Real p)
{
   elements.SMA = a;
   elements.ALT = alt;
   elements.ECC = e;
   elements.INC = i;
   elements.ROP = rop;
   elements.ROA = roa;
   elements.P   = p;

   errormsg     = "";
   isError      = false;
}

//------------------------------------------------------------------------------
// ~SunSync()
//------------------------------------------------------------------------------
/**
 * This method destroys the object of the SunSync class
 * (destructor).
 *
 */
//------------------------------------------------------------------------------
SunSync::~SunSync()
{
}

//------------------------------------------------------------------------------
// void CalculateSunSync(bool aVal, Real a, bool altVal, Real alt,
//                       bool eVal,  Real e, bool iVal, Real i, bool ropVal,
//                       Real RoP,  bool roaVal, Real RoA, bool pVal,
//                       Real P)
//------------------------------------------------------------------------------
/**
 * Calculates the SunSync values.
 *
 */
//------------------------------------------------------------------------------
void SunSync::CalculateSunSync(bool aVal, Real a, bool altVal, Real alt, 
	                            bool eVal,  Real e, bool iVal, Real i, bool ropVal,
							          Real RoP,  bool roaVal, Real RoA, bool pVal,
							          Real P)
{
   errormsg = "";
   isError  = false;

   //check if 2 inputs given
   if ((aVal+eVal+iVal+ropVal+roaVal+pVal+altVal)<2)
   {
	  a = -1;
	  isError = true;
	  errormsg = "Minimum of 2 inputs necessary";
	  return;
   }

   //check if values are in sun synchronous range
   if (aVal)
   {
      if ((a<(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100.0)) || 
		  (a>(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000.0))) 
	  {
	     isError = true;
		 errormsg = "SMA out of bounds, 6478.1363 < SMA < 8378.1363";
		 return;
	  }
   }
   if (altVal)
   {
	  if ((alt<100.0) || (alt>2000.0)) 
	  {
	     isError = true;
		 errormsg = "ALT out of bounds, 100 < ALT < 2000";
		 return;
	  }
   }
   if (eVal)
   {
      if ((e<0.0) || (e>=1.0))
	  {
	     isError = true;
		 errormsg = "ECC out of bounds, 0 < ECC < 1";
		 return;
	  }
   }
   if (iVal)
   {
      if ((i<90.0) || (i>110.0))
	  {
	     isError = true;
		 errormsg = "INC out of bounds, 90 < INC < 110";
		 return;
	  }
   }
   if (ropVal)
   {
      if ((RoP<(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)) ||
		  (RoP>(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000)))
	  {
	     isError = true;
		 errormsg = "RP out of bounds, 6478.1363 < RP < 8378.1363";
		 return;
	  }
   }
   if (roaVal)
   {
      if ((RoA < (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)) ||
		  (RoA > ((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000)*2 
		   - (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100))))
	  {
	     isError = true;
		 errormsg = "RA out of bounds, 6478.1363 < RA < 10278.1363";
		 return;
	  }
   }
   if (pVal)
   {
      if (P<=0 || P>(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000))
	  {
	     isError = true;
		 errormsg = "P out of bounds, 0 < P < 8378.1363";
		 return;
	  }
   }

   //if alt given, find sma
   if (altVal)
   {
       a = alt + GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2];
       aVal = 1;
   }

   //if given radius of apoapsis or periapsis, find a or e
   if (roaVal)
   {
      if (ropVal)
	  {
	     if (RoA<RoP)
		 {
		    isError = true;
		    errormsg = "RA must be greater than RP";
			return;
		 }
         e = (RoA - RoP)/(RoA+RoP);
         a = RoA/(1+e);
		 /*if ((a<(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100.0)) || 
		     (a>(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000.0))) 
		 {
			isError = true;
			errormsg = 
			   "No sun synchronous orbit can be found with those parameters, try with "
			   + GmatStringUtil::Trim(GmatStringUtil::ToString((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100.0)*(1+e)))
			   + " < RA < " +  
			   GmatStringUtil::Trim(GmatStringUtil::ToString((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000.0)*(1+e)));
			return;
		 }*/
		 if ((e>=1)||(e<0))
		 {
		    isError = true;
			errormsg = "No sun synchronous orbit can be found with those parameters";
			return;
		 }
	  }
      else if (aVal)
	  {
	     if (altVal && 
			 ((alt+GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2])>RoA))
		 {
		    isError = true;
		    errormsg = 
			   "Alt must be less than " + 
			   GmatStringUtil::Trim(GmatStringUtil::ToString(RoA - 
			   GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]));
			return;
		 }
		 if (RoA<a)
		 {
		    isError = true;
		    errormsg = "RA must be greater than SMA";
			return;
		 }
         e = -1 + (RoA/a);
		 if ((e>=1)||(e<0))
		 {
		    isError = true;
			errormsg = "No sun synchronous orbit can be found with those parameters";
			return;
		 }
      }
      else if (eVal)
	  {
         a = RoA/(1+e);
		 if ((a<(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100.0)) || 
		     (a>(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000.0))) 
		 {
			isError = true;
			errormsg = 
			   "No sun synchronous orbit can be found with those parameters, try with "
			   + GmatStringUtil::Trim(GmatStringUtil::ToString((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100.0)*(1+e)))
			   + " < RA < " +  
			   GmatStringUtil::Trim(GmatStringUtil::ToString((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000.0)*(1+e)));
			return;
		 }
	  }
      else if (pVal)
	  {
		 if (RoA<P)
		 {
		    isError = true;
		    errormsg = "RA must be greater than P";
			return;
		 }
		 e = 1 - (P/RoA);
	     a = RoA/(1+e);
		 if ((e>=1)||(e<0))
		 {
		    isError = true;
			errormsg = "No sun synchronous orbit can be found with those parameters";
			return;
		 }
		 /*if ((a<(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100.0)) || 
		     (a>(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000.0))) 
		 {
			isError = true;
			errormsg = 
			   "No sun synchronous orbit can be found with those parameters, try with "
			   + GmatStringUtil::Trim(GmatStringUtil::ToString((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100.0)*(1+e)))
			   + " < RA < " +  
			   GmatStringUtil::Trim(GmatStringUtil::ToString((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000.0)*(1+e)));
			return;
		 }*/
	  }
   }
   else if (ropVal)
   {
      if (aVal)
	  {
	     if (altVal && 
			 ((alt+GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2])<RoP))
		 {
		    isError = true;
		    errormsg = "Alt must be greater than " + 
			   GmatStringUtil::Trim(GmatStringUtil::ToString(RoP - 
			   GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]));
			return;
		 }
		 if (RoP>a)
		 {
		    isError = true;
		    errormsg = "RP must be less than SMA";
			return;
		 }
         e = 1 - (RoP/a);
		 if ((e>=1)||(e<0))
		 {
		    isError = true;
			errormsg = "No sun synchronous orbit can be found with those parameters";
			return;
		 }
	  }
      else if (eVal)
	  {
         a = RoP/(1-e);
		 if ((a<(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100.0)) || 
		     (a>(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000.0))) 
		 {
			isError = true;
			errormsg = 
			   "No sun synchronous orbit can be found with those parameters, try with RP < " 
			   + GmatStringUtil::Trim(GmatStringUtil::ToString(
			   (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000.0)*(1-e))) + 
			   ", or with ECC < " + GmatStringUtil::Trim(GmatStringUtil::ToString(1-
			   (RoP/(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000.0))));
		 }
	  }
      else if (pVal)
      {
		 if (RoP>P)
		 {
		    isError = true;
			errormsg = "RP must be less than P";
			return;
		 }
		 e = (P/RoP)-1;
         a = RoP/(1-e);
		 if ((e>=1)||(e<0))
		 {
		    isError = true;
			errormsg = "No sun synchronous orbit can be found with those parameters, try with RP > "
				        + GmatStringUtil::Trim(GmatStringUtil::ToString(P/2)) +  ", or with P < "
						+ GmatStringUtil::Trim(GmatStringUtil::ToString(RoP*2));
			return;
		 }
		 /*if ((a<(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100.0)) || 
		     (a>(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000.0))) 
		 {
			isError = true;
			errormsg = "No sun synchronous orbit can be found with those parameters, try with RP > "
			   + GmatStringUtil::Trim(GmatStringUtil::ToString((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]
			   +2000.0)*(1-e))) +  ", or with P < "
			   + GmatStringUtil::Trim(GmatStringUtil::ToString((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]
			   +2000.0)*(1-pow(e,2))));
			return;
		 }*/
	  }
   }
   else if (pVal)
   {
      if (aVal)
	  {
		 if ((P/a)>1)
		 {
		    isError = true;
			errormsg = "P cannot be greater than SMA";
			return;
		 }
         e = sqrt(1-(P/a));
		 if ((e>=1)||(e<0))
		 {
		    isError = true;
			errormsg = "No sun synchronous orbit can be found with those parameters";
			return;
		 }
	  }
      else if (eVal)
	  {
         a = P/(1-pow(e,2));
		 if ((a<(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100.0)) || 
		     (a>(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000.0))) 
		 {
			isError = true;
			errormsg = "No sun synchronous orbit can be found with those parameters";
			return;
		 }
	  }
   }

   //if values in range, find orbit
   if (isError)
   {
      a = -1; 
      e = -1;
      i = -1;
   }
   //find a from e, i
   else if (eVal && iVal)
      a = SolveA(e, i*GmatMathConstants::PI/180,1);
   //find e from a, i
   else if (aVal && iVal)
	  if (altVal)
		 e = SolveE(a, i*GmatMathConstants::PI/180, 1, true);
	  else
         e = SolveE(a, i*GmatMathConstants::PI/180, 1);  
   //find i from a, e
   else if (ropVal && roaVal)
      i = SolveI(a, e, 1, true, true, false, false, false);
   else if (ropVal && aVal)
      if (altVal)
		 i = SolveI(a, e, 1, false, true, false, true, false, true);
	  else
         i = SolveI(a, e, 1, false, true, false, true);
   else if (ropVal && eVal)
      i = SolveI(a, e, 1, false, true, true, false);
   else if (roaVal && aVal)
      if (altVal)
		 i = SolveI(a, e, 1, true, false, false, true, false, true);
	  else
         i = SolveI(a, e, 1, true, false, false, true);
   else if (roaVal && eVal)
      i = SolveI(a, e, 1, true, false, true, false);
   else if (pVal && eVal)
      i = SolveI(a, e, 1, false, false, true, false, true);
   else if (pVal && aVal)
      if (altVal)
	     i = SolveI(a, e, 1, false, false, false, true, true, true);
	  else
         i = SolveI(a, e, 1, false, false, false, true, true);
   else if (pVal && ropVal)
      i = SolveI(a, e, 1, false, true, false, false, true);
   else if (pVal && roaVal)
	  i = SolveI(a, e, 1, true, false, false, false, true);
   else if (aVal && eVal)
	  if (altVal)
		 i = SolveI(a, e, 1, false, false, true, true, false, true);
	  else
         i = SolveI(a, e, 1);   
   //find a and e from P and i
   else if (pVal && iVal)
	  SolvePAE(P, i*GmatMathConstants::PI/180, a, e);
   //find a and e from RoA and i
   else if (roaVal && iVal)
	  SolveRoAAE(RoA, i*GmatMathConstants::PI/180, a, e);
   //find a and e from RoP and i
   else if (ropVal && iVal)
      SolveRoPAE(RoP, i*GmatMathConstants::PI/180, a, e);

   //if orbit is found, calculate remaining parameters
   if (!altVal && !isError)
      alt = a - GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2];
   else if (isError)
      alt = -1;
   if (!roaVal && !isError)
      RoA = a*(1+e);
   else if (isError)
      RoA = -1;
   if (!ropVal && !isError)
      RoP = a*(1-e);
   else if (isError)
      RoP = -1;
   if (!pVal && !isError)
      P = a*(1-pow(e,2));
   else if (isError)
      P = -1;

   elements.SMA = a;
   elements.ALT = alt;
   elements.ECC = e;
   elements.INC = i;
   elements.ROP = RoP;
   elements.ROA = RoA;
   elements.P = P;
}

//------------------------------------------------------------------------------
// Real GetSMA()
//------------------------------------------------------------------------------
/**
 * Returns SMA value.
 *
 */
//------------------------------------------------------------------------------
Real SunSync::GetSMA()
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
Real SunSync::GetALT()
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
Real SunSync::GetECC()
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
Real SunSync::GetINC()
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
Real SunSync::GetROA()
{
   return elements.ROA;
}

//------------------------------------------------------------------------------
// Real GetROP()
//------------------------------------------------------------------------------
/**
 * Returns ROP value.
 *
 */
//------------------------------------------------------------------------------
Real SunSync::GetROP()
{
   return elements.ROP;
}

//------------------------------------------------------------------------------
// Real GetP()
//------------------------------------------------------------------------------
/**
 * Returns P value.
 *
 */
//------------------------------------------------------------------------------
Real SunSync::GetP()
{
   return elements.P;
}

//------------------------------------------------------------------------------
// bool IsError()
//------------------------------------------------------------------------------
/**
 * Returns indicating whether or not there has been an error.
 *
 */
//------------------------------------------------------------------------------
bool SunSync::IsError()
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
std::string SunSync::GetError()
{
	return errormsg;
}

//------------------------------------------------------------------------------
// Real SolveA(Real e, Real i, bool flag)
//------------------------------------------------------------------------------
/**
 * Solves for A, given inputs.
 *
 */
//------------------------------------------------------------------------------
Real SunSync::SolveA(Real e, Real i, bool flag)
{
   Real a, e_low, e_high, rp, e_rp, a_1, a_0, i_low, i_high;
   int count;
   Real J2 = 0.0010826267;
   Real omegaSunSync = (2*GmatMathConstants::PI/365.2422)/86400;

   //fixes warnings
   a_1 = 0;

   double ex = 2.0/7;
   a = 
      pow((-3*sqrt(GmatSolarSystemDefaults::PLANET_MU[2])*
	  pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*
	  J2*cos(i))/(2*omegaSunSync*pow(1-pow(e,2),2)),ex);
    
   //if solutions out of range, give upper and lower input bounds 
   //to satisfy requirements
   if (((a < (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)) || 
	   (a>(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000)) || 
	   (a*(1-e) < (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100))) 
	   && !(a==-1) && flag)
   {
      e_low = 
	     SolveE(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100,i,0);
      if (e_low == -1) 
	     e_low = 0;
      rp = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100;
      e_rp = 1 - (rp/a);
      e_high = 
	     SolveE(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000,i,0);
      if (e_high == -1) 
	     e_high = 1;
      if (e_rp<e_high) 
	     e_high = e_rp;
      if (e_low>e_high)
	   {
         double x = e_low; 
		   e_low    = e_high;
		   e_high   = x;
	  }
      if (e_low<0) 
	  {
	     e_low = 0;
        a_1   = 1;
		  a_0   = a;
		  count = 0;
	  }
      while ((a_1*(1-e_high) < 
		     (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100))
		     && count<15)
	  {
         e_high = 1 -(rp/a_0);
         a_1 = SolveA(e_high, i,0);
         e_high = SolveE(a_1, i, 0);
         a_0 = a_1;
         count = count +1;
	  }
      i_low = 
	     SolveI((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000),e,0);
      if (i_low == -1) 
	     i_low = 90;
      i_high = 
	     SolveI((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100),e,0);
      if (i_high == -1) 
	     i_high = 110;
      if (i_low>i_high)
	  {
         double x = i_high; 
		 i_high = i_low; 
		 i_low = x;
	  }
      a = -1;
	  //if e or i is in the calculated bounds, do not suggest input
	  if (((i*180/GmatMathConstants::PI)>i_low)&&((i*180/GmatMathConstants::PI)<i_high))
	  {
	     if ((e>e_low)&&(e<e_high))
		    errormsg = 
	           "No sun synchronous orbit can be found with those parameters";
		 else
            errormsg = 
	           "No sun synchronous orbit can be found with those parameters, try with " 
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(e_low)) + " < ECC < " + 
		       GmatStringUtil::Trim(GmatStringUtil::ToString(e_high));
	  }
	  else if ((e>e_low)&&(e<e_high))
	  {
         errormsg = 
	        "No sun synchronous orbit can be found with those parameters, try with " 
		    + GmatStringUtil::Trim(GmatStringUtil::ToString(i_low)) + " < INC < " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
	  }
	  else
	  {
	     errormsg = 
	        "No sun synchronous orbit can be found with those parameters, try with " 
		    + GmatStringUtil::Trim(GmatStringUtil::ToString(e_low)) + " < ECC < " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(e_high)) + ", or try " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(i_low)) + " < INC < " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
	  }
      isError = true;
   }
   
   return a;
}

//------------------------------------------------------------------------------
// Real SolveE(Real a, Real i, bool flag, bool altFlag)
//------------------------------------------------------------------------------
/**
 * Solves for E, given inputs.
 *
 */
//------------------------------------------------------------------------------
Real SunSync::SolveE(Real a, Real i, bool flag, bool altFlag)
{
   Real e, check, i_high, i_low, a_high, rp, e_rp, a_low;
   Real J2 = 0.0010826267;
   Real omegaSunSync = (2*GmatMathConstants::PI/365.2422)/86400;
    
   //check if e exists with given i, a
   check = 
      (-3*sqrt(GmatSolarSystemDefaults::PLANET_MU[2])*
	  pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*
	  J2*cos(i))/(2*omegaSunSync*pow(a,3.5));
   if (check < 0)
   {
      errormsg = 
	     "No sun synchronous orbit can be found with those parameters, try with 90 < INC < 110";
	  isError = true;
      e = -1;
	  return e;
   }
   else if (sqrt(check) > 1)
   {
      a_high = 
	     pow(-3*sqrt(GmatSolarSystemDefaults::PLANET_MU[2])*
		 pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*
		 J2*cos(i)/(2*omegaSunSync),3.5);
      if (a_high > (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000)) 
	     a_high = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000;
      i_high = 
	     acos(2*omegaSunSync*pow(a,3.5)/
		 (-3*sqrt(GmatSolarSystemDefaults::PLANET_MU[2])*
		 pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*J2))*
		 180/GmatMathConstants::PI;
	  if (a<a_high)
	  {
	     if ((i*180/GmatMathConstants::PI)<i_high)
	        errormsg = 
	           "No sun synchronous orbit can be found with those parameters";
		 else
		    errormsg = 
	           "No sun synchronous orbit can be found with those parameters, try with INC < "
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
	  }
	  else if ((i*180/GmatMathConstants::PI)<i_high)
	  {
		 if (altFlag)
			errormsg = 
	           "No sun synchronous orbit can be found with those parameters, try with ALT < "
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(a_high - 
			   GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]));
		 else
	        errormsg = 
	           "No sun synchronous orbit can be found with those parameters, try with SMA < "
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(a_high));
	  }
	  else
	  {
		 if (altFlag)
			errormsg = 
	           "No sun synchronous orbit can be found with those parameters, try with ALT < "
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(a_high - 
			   GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2])) + " or with INC < " 
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
		 else
            errormsg = 
	           "No sun synchronous orbit can be found with those parameters, try with SMA < "
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(a_high)) + " or with INC < " 
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
	  }
	  isError = true;
      e = -1;
      return e;
   }
   else  
   {
      e = sqrt(1-sqrt(check));
   }
    
   if (((e >= 1) || (e < 0) || 
	   (a*(1-e) < (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100))) 
	   && flag && !(e==-1))
   {
      rp = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100;
      e_rp = 1 - (rp/a);
      a_high = SolveA(0,i,0);
      if (a_high == -1 || 
		  a_high>(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000) ||
		  a_high<(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)) 
	     a_high = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000;
      a_low = SolveA(e_rp,i,0);
      if (a_low < GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100) 
	     a_low = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100;
      i_high = SolveI(a,0,0);
      if (i_high == -1) 
	     i_high = 110;
      i_low = SolveI(a,e_rp,0);
      if (i_low == -1) 
	     i_low = 90;
      e = -1;
      //if a or e is in the calculated bounds, do not suggest input
	  if ((a>a_low)&&(a<a_high))
	  {
	     if (((i*180/GmatMathConstants::PI)>i_low)&&((i*180/GmatMathConstants::PI)<i_high))
		    errormsg = 
	           "No sun synchronous orbit can be found with those parameters";
		 else
		    errormsg = 
	           "No sun synchronous orbit can be found with those parameters, try with "
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(i_low)) + " < INC < " 
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
	  }
	  else if (((i*180/GmatMathConstants::PI)>i_low)&&((i*180/GmatMathConstants::PI)<i_high))
	  {
		 if (altFlag)
			errormsg = 
	           "No sun synchronous orbit can be found with those parameters, try with "
		       + GmatStringUtil::Trim(GmatStringUtil::Trim(GmatStringUtil::ToString(a_low - 
			   GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2])))
			   + " < ALT < " + GmatStringUtil::Trim(GmatStringUtil::ToString(a_high - 
			   GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]));
		 else
	        errormsg = 
	           "No sun synchronous orbit can be found with those parameters, try with "
		       + GmatStringUtil::Trim(GmatStringUtil::Trim(GmatStringUtil::ToString(a_low)))
			   + " < SMA < " + GmatStringUtil::Trim(GmatStringUtil::ToString(a_high));
	  }
	  else
	  {
		 if (altFlag)
			errormsg = 
	           "No sun synchronous orbit can be found with those parameters, try with "
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(a_low - 
			   GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2])) + " < ALT < " 
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(a_high - 
			   GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2])) + ", or try " 
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(i_low)) + " < INC < " 
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
		 else
            errormsg = 
	           "No sun synchronous orbit can be found with those parameters, try with "
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(a_low)) + " < SMA < " 
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(a_high)) + ", or try " 
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(i_low)) + " < INC < " 
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
	  }
	  isError = true;
   }

   return e;
}

//------------------------------------------------------------------------------
// Real SolveI(Real a, Real e, bool flag, bool roaFlag, bool ropFlag,
//             bool eFlag, bool aFlag, bool pFlag, bool altFlag)
//------------------------------------------------------------------------------
/**
 * Solves for I, given inputs.
 *
 */
//------------------------------------------------------------------------------
Real SunSync::SolveI(Real a, Real e, bool flag, bool roaFlag, bool ropFlag, 
	                 bool eFlag, bool aFlag, bool pFlag, bool altFlag)
{
   Real i, check, aLow, aHigh, eLow, eHigh, rp, eRp, roaLow, roaHigh;
   Real ropHigh, ropLow, pLow, pHigh;
   int count = 0;
   Real J2 = 0.0010826267;
   Real omegaSunSync = (2*GmatMathConstants::PI/365.2422)/86400;

   //check if i exists with given i, e
   check = 
      (2*omegaSunSync*pow((1-pow(e,2)),2)*pow(a,(3.5)))/
	  (-3*sqrt(GmatSolarSystemDefaults::PLANET_MU[2])*
	  pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*J2);
   if ((check < -1) || (check >1))
   {
      errormsg = 
	     "No sun synchronous orbit can be found with those parameters";
	  isError = true;
      i = -1;
	  return i;
   }
   else
   {
      i = acos(check)*180/GmatMathConstants::PI;
   }
     
   //if solutions out of range, give upper and lower input bounds 
   //to satisfy requirements
   if (((i < 90) || (i > 110) || 
        (a*(1-e) < (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100))) ||
       ((a > (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000)) && flag && !(i==-1)))
   {
	  eHigh = SolveE(a,90.000000000001*GmatMathConstants::PI/180,0);
      if (eHigh == -1) 
	     eHigh = 1;
      rp = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100;
      eRp = 1 - (rp/a);
      if (eRp<eHigh) 
	     eHigh = eRp;
      eLow = SolveE(a,110*GmatMathConstants::PI/180,0);
      if (eLow <= 0) 
	     eLow = 0;
      aLow = SolveA(e,90.000000000001*GmatMathConstants::PI/180,0);
      if (aLow < (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)) 
	     aLow = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100;
	  if (aLow > (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000))
		 aLow = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000;
	  if ((aLow*(1-e))<rp)
		 aLow = rp/(1-e);
      aHigh = SolveA(e,110*GmatMathConstants::PI/180,0);
      if ((aHigh == -1) || 
		  (aHigh > GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000)) 
	     aHigh = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000;
      
	  pLow = aLow*(1-pow(eHigh,2));
	  pHigh = aHigh*(1-pow(eLow,2));

	  //if a or e is in the calculated bounds, do not suggest input
	  if (ropFlag && roaFlag)
	  {
		 errormsg = 
	        "No sun synchronous orbit can be found with those parameters";
		 if (a>(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000))
		 {
			roaHigh = 2*(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000) 
				      - (a*(1-e));
			roaLow = a*(1-e);
			ropHigh = 2*(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000) 
				      - (a*(1+e));
			ropLow = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100;
		    errormsg = 
	           "No sun synchronous orbit can be found with those parameters, try with "
			   + GmatStringUtil::Trim(GmatStringUtil::ToString(roaLow)) + " < RA < "
			   + GmatStringUtil::Trim(GmatStringUtil::ToString(roaHigh)) + ", or with "
			   + GmatStringUtil::Trim(GmatStringUtil::ToString(ropLow)) + " < RP < "
			   + GmatStringUtil::Trim(GmatStringUtil::ToString(ropHigh)); 
		 }
	  }
	  else if (ropFlag)
	  {
	     if (eFlag)
		 {
			ropHigh = aHigh*(1-e);
			ropLow = aLow*(1-e);
			if (ropLow<GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)
			   ropLow = (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100);
			eHigh = 1 - (a*(1-e)/aHigh);
			eLow = 0;
		    if ((((a*(1-e))>ropLow)&&(ropHigh<(a*(1-e))))||((e>eLow)&&(e<eHigh)))
			   errormsg = 
	              "No sun synchronous orbit can be found with those parameters";
		    else
	           errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with ?"
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(ropLow)) + " < RP, or try " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(eLow)) + " < ECC < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(eHigh));
		 }
		 else if (aFlag)
		 {
			ropHigh = a*(1-eLow);
			ropLow = a*(1-eHigh);
		    if (((a>aLow)&&(a<aHigh))||(((a*(1-e))>ropLow)&&(ropHigh<(a*(1-e)))))
			   errormsg = 
	              "No sun synchronous orbit can be found with those parameters";
		    else if (altFlag)
	           errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(ropLow)) + " < RP, or try " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aLow - 
				  GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2])) + " < ALT < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aHigh - 
				  GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]));
			else
			   errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(ropLow)) + " < RP, or try " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aLow)) + " < SMA < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aHigh));
		 }
		 else if (pFlag)
		 {
			pLow = a*(1-e);//RP
			aHigh = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000;
			eHigh = sqrt(1-(a*(1-pow(e,2)))/aHigh);
			ropLow = aHigh*(1-eHigh);
			eHigh = 1-(a*(1-e)/aHigh);
			pHigh = aHigh*(1-pow(eHigh,2));
			ropHigh = a*(1-pow(e,2));//P

		    if (((a>aLow)&&(a<aHigh))||((e>eLow)&&(e<eHigh)))
			   errormsg = 
	              "No sun synchronous orbit can be found with those parameters";
		    else
	           errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(pLow)) + " < P < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(pHigh)) + ", or try " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(ropLow)) + " < RP < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(ropHigh));
			
		 }
	  }
	  else if (roaFlag)
	  {
	     if (eFlag)
		 {
			roaHigh = aHigh*(1+e);
			if (aLow*(1-e)<GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)
			   aLow = (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)/(1-e);
			roaLow = aLow*(1+e);
			eHigh = (a/aLow)*(1+e)-1;
			if (eHigh<0)
			   eHigh = 1;

			if (((a*(1+e))>roaLow)&&(roaHigh<(a*(1+e))))
               errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(eLow)) + " < ECC < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(eHigh));
			else if ((e>eLow)&&(e<eHigh))
               errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(roaLow)) + " < RA < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(roaHigh));
			else if (((((a*(1+e))>roaLow)&&(roaHigh<(a*(1+e)))))||((e>eLow)&&(e<eHigh)))
			   errormsg = 
	              "No sun synchronous orbit can be found with those parameters";
		    else
	           errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(roaLow)) + " < RA < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(roaHigh)) + ", or try " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(eLow)) + " < ECC < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(eHigh));
		 }
		 else if (aFlag)
		 {
			roaHigh = a*(1+eHigh);
			roaLow = a*(1+eLow);

			aHigh = a*(1+e)/(1+eLow);
			if (aHigh>(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000))
			   aHigh = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000;

			aLow = a*(1+e)/(1+eHigh);
			if (aLow<(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100))
			   aLow = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100;

			if (((a>aLow) && (a<aHigh)) || (((a*(1+e)) < roaLow) && (roaHigh<(a*(1+e)))))
			   errormsg = 
	              "No sun synchronous orbit can be found with those parameters";
		    else if (altFlag)
			   errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(roaLow)) + " < RA < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(roaHigh)) + ", or try " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aLow - 
				  GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2])) + " < ALT < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aHigh - 
				  GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]));
			else
	           errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(roaLow)) + " < RA < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(roaHigh)) + ", or try " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aLow)) + " < SMA < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aHigh));
		 }
		 else if (pFlag)
		 {
			pHigh = a*(1+e);//ra
			eHigh = (a*(1+e)/(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000))-1;
			if (pHigh > (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000))
			   pHigh = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000;
			if ((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000)*(1-pow(eHigh,2))<pHigh)
			   pHigh = (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000)*(1-pow(eHigh,2));
			count = 0;
			aHigh = a;
			eHigh = e;
			while ((aHigh*(1-eHigh) < (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100))&&(count<1000))
			{
			   eHigh = 1-((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)/aHigh);
               aHigh = a*(1- pow(e,2))/(1-pow(eHigh,2));
			   count++;
			}
			roaHigh = aHigh*(1+eHigh);
			
			count = 0;
			aHigh = a;
			eHigh = e;
			/*while (((aHigh*(1-eHigh) < (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100))||(aHigh*(1+eHigh) > a*(1+e)))&&(count<10000))
			{
			   eHigh = 1-((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)/aHigh);
               aHigh = a*(1- pow(e,2))/(1-pow(eHigh,2));
			   count++;
			}*/
			pLow = aHigh*(1-pow(eHigh,2));

			roaLow = a*(1-pow(e,2));//P
			if (roaLow < (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100))
			   roaLow = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100;

		    if (((a>aLow)&&(a<aHigh))||((e>eLow)&&(e<eHigh)))
			   errormsg = 
	              "No sun synchronous orbit can be found with those parameters";
		    else
	           errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(pLow)) + " < P < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(pHigh)) + ", or try " 
				  + GmatStringUtil::Trim(GmatStringUtil::ToString(roaLow)) +" < RA < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(roaHigh));
		 }
	  }
	  else if (pFlag)
	  {
	     if (eFlag)
		 {
			pLow = aLow*(1-pow(e,2));
	        pHigh = (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000)*(1-pow(e,2));
			count = 0;
			aHigh = a;
			eHigh = e;
			while ((aHigh*(1-eHigh) < (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100))&&(count<1000))
			{
			   eHigh = 1-((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)/aHigh);
               aHigh = a*(1- pow(e,2))/(1-pow(eHigh,2));
			   count++;
			}

			if ((((a*(1-pow(e,2)))>pLow)&&((a*(1-pow(e,2)))<pHigh)) || ((e>eLow)&&(e<eHigh)))
			   errormsg = 
	              "No sun synchronous orbit can be found with those parameters";
			else
	           errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(pLow)) + " < P < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(pHigh)) + ", or try " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(eLow)) + " < ECC < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(eHigh));
		 }
		 else if (aFlag)
		 {
			pLow = a*(1-pow(eHigh,2));
	        pHigh = a*(1-pow(eLow,2));
			aLow = a*(1-pow(e,2));
			if (aLow<(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100))
			   aLow = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100;
			count = 0;
			aHigh = a;
			eHigh = e;
			while ((aHigh*(1-eHigh) < (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100))&&(count<1000))
			{
			   eHigh = 1-((GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)/aHigh);
               aHigh = a*(1- pow(e,2))/(1-pow(eHigh,2));
			   count++;
			}

			if ((((a*(1-pow(e,2)))>pLow)&&((a*(1-pow(e,2)))<pHigh)) || ((e>eLow)&&(e<eHigh)))
			   errormsg = 
	              "No sun synchronous orbit can be found with those parameters";
			else if (altFlag)
			   errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(pLow)) + " < P < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(pHigh)) + ", or try " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aLow - 
				  GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2])) + " < ALT < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aHigh - 
				  GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]));
		    else
	           errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(pLow)) + " < P < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(pHigh)) + ", or try " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aLow)) + " < SMA < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aHigh));
			MessageInterface::ShowMessage("e: %f\n", e);
		 MessageInterface::ShowMessage("a: %f\n", a);
		 MessageInterface::ShowMessage("i: %f\n", i);
		 MessageInterface::ShowMessage("eHigh: %f\n", eHigh);
		 MessageInterface::ShowMessage("aHigh: %f\n", aHigh);
		 MessageInterface::ShowMessage("pHigh: %f\n", pHigh);
		 MessageInterface::ShowMessage("aLow: %f\n", aLow);
		 MessageInterface::ShowMessage("pLow: %f\n", pLow);
		 }
	  }
	  else
	  {
	     if ((a>aLow)&&(a<aHigh))
	     {
	        if ((e>eLow)&&(e<eHigh))
		       errormsg = 
	              "No sun synchronous orbit can be found with those parameters";
		    else
		       errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(eLow)) + " < ECC < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(eHigh));
	     }
	     else if ((e>eLow)&&(e<eHigh))
	     {
			if (altFlag)
			   errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::Trim(GmatStringUtil::ToString(aLow - 
				  GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2])))
			      + " < ALT < " + GmatStringUtil::Trim(GmatStringUtil::ToString(aHigh - 
				  GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]));
			else
			   if (altFlag)
				  errormsg = 
	                 "No sun synchronous orbit can be found with those parameters, try with "
		             + GmatStringUtil::Trim(GmatStringUtil::Trim(GmatStringUtil::ToString(aLow- 
				     GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2])))
			         + " < ALT < " + GmatStringUtil::Trim(GmatStringUtil::ToString(aHigh- 
				     GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]));
			   else
	              errormsg = 
	                 "No sun synchronous orbit can be found with those parameters, try with "
		             + GmatStringUtil::Trim(GmatStringUtil::Trim(GmatStringUtil::ToString(aLow)))
			         + " < SMA < " + GmatStringUtil::Trim(GmatStringUtil::ToString(aHigh));
	     }
	     else
	     {
			if (altFlag)
			   errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aLow - 
				  GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2])) + " < ALT < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aHigh - 
				  GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2])) + ", or try " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(eLow)) + " < ECC < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(eHigh));
			else
               errormsg = 
	              "No sun synchronous orbit can be found with those parameters, try with "
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aLow)) + " < SMA < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(aHigh)) + ", or try " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(eLow)) + " < ECC < " 
		          + GmatStringUtil::Trim(GmatStringUtil::ToString(eHigh));
	     }
	  }

	  isError = true;
	  i = -1;
   }

   return i;
}


//------------------------------------------------------------------------------
// Real SolvePAE(Real P, Real i, Real &a, Real &e)
//------------------------------------------------------------------------------
/**
 * Solves for PAE, given inputs.
 *
 */
//------------------------------------------------------------------------------
void SunSync::SolvePAE(Real P, Real i, Real &a, Real &e)
{
   Real n, n_lo, n_hi, P_low, P_high, i_high, i_low, check;
   Real J2 = 0.0010826267;
   Real omegaSunSync = (2*GmatMathConstants::PI/365.2422)/86400;

   n = 
      -2*omegaSunSync*pow(P,2)/
	  (3*J2*pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*cos(i));
   a = pow(GmatSolarSystemDefaults::PLANET_MU[2]/pow(n,2),1.0/3);

   //if solutions out of range, give upper and lower input bounds to 
   //satisfy requirements
   if (a>(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000) ||
	   a<(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100))
   {
      n_lo = 
	     sqrt(GmatSolarSystemDefaults::PLANET_MU[2]/
		 pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000,3));
      n_hi = 
		 sqrt(GmatSolarSystemDefaults::PLANET_MU[2]/
		 pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100,3));
      P_low = 
		 sqrt(n_lo*3*J2*
		 pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*cos(i)/
		 (-2*omegaSunSync));
      if (P_low > (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000))  
	     P_low = 0;
      P_high = 
	     sqrt(n_hi*3*J2*
		 pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*cos(i)/
		 (-2*omegaSunSync));
      if (P_high > (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000)) 
	     P_high = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000;
      i_high = 
		 acos(-2*omegaSunSync*pow(P,2)/
		 (3*J2*pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*n_lo))*
		 180/GmatMathConstants::PI;
      i_low = 
		 acos(-2*omegaSunSync*pow(P,2)/
		 (3*J2*pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*n_hi))*
		 180/GmatMathConstants::PI;
      e = -1;
	  //if P or i is in the calculated bounds, do not suggest input
	  if (((i*180/GmatMathConstants::PI)>i_low)&&((i*180/GmatMathConstants::PI)<i_high))
	  {
	     if ((P>P_low)&&(P<P_high))
		    errormsg = 
	           "No sun synchronous orbit can be found with those parameters";
		 else
            errormsg = 
	           "No sun synchronous orbit can be found with those parameters, try with " 
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(P_low)) + " < P < " + 
		       GmatStringUtil::Trim(GmatStringUtil::ToString(P_high));
	  }
	  else if ((P>P_low)&&(P<P_high))
	  {
         errormsg = 
	        "No sun synchronous orbit can be found with those parameters, try with " 
		    + GmatStringUtil::Trim(GmatStringUtil::ToString(i_low)) + " < INC < " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
	  }
	  else
	  {
	     errormsg = 
	        "No sun synchronous orbit can be found with those parameters, try with " 
		    + GmatStringUtil::Trim(GmatStringUtil::ToString(P_low)) + " < P < " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(P_high)) + ", or try " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(i_low)) + " < INC < " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
	  }
	  isError = true;
	  return;
   }

   check = P/a;
   if ((check <=1) && (check >0)) 
   {
	  e = sqrt(1-(P/a));
   }
   else
   {
      //if solutions out of range, give upper and lower input bounds 
	  //to satisfy requirements
      n_hi = 
	     sqrt(GmatSolarSystemDefaults::PLANET_MU[2]/
		 pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100,3));
      n_lo = 
		 sqrt(GmatSolarSystemDefaults::PLANET_MU[2]/
		 pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000,3));
      P_low = 
		 sqrt(n_lo*3*J2*
		 pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*cos(i)/
		 (-2*omegaSunSync));
      if (P_low > (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000))  
	     P_low = 0;
      P_high = 
		 sqrt(n_hi*3*J2*
		 pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*cos(i)/
		 (-2*omegaSunSync));
      if (P_high > a) 
	     P_high = a;
      i_high = 
		 acos(-2*omegaSunSync*pow(P,2)/
		 (3*n_lo*J2*pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)))*
		 180/GmatMathConstants::PI;
      i_low = 
		 acos(-2*omegaSunSync*pow(P_low,2)/
		 (3*n_hi*J2*pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)))*
		 180/GmatMathConstants::PI;
      e = -1;
      //if P or i is in the calculated bounds, do not suggest input
	  if (((i*180/GmatMathConstants::PI)>i_low)&&((i*180/GmatMathConstants::PI)<i_high))
	  {
	     if ((P>P_low)&&(P<P_high))
		    errormsg = 
	           "No sun synchronous orbit can be found with those parameters";
		 else
            errormsg = 
	           "No sun synchronous orbit can be found with those parameters, try with " 
		       + GmatStringUtil::Trim(GmatStringUtil::ToString(P_low)) + " < P < " + 
		       GmatStringUtil::Trim(GmatStringUtil::ToString(P_high));
	  }
	  else if ((P>P_low)&&(P<P_high))
	  {
         errormsg = 
	        "No sun synchronous orbit can be found with those parameters, try with " 
		    + GmatStringUtil::Trim(GmatStringUtil::ToString(i_low)) + " < INC < " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
	  }
	  else
	  {
	     errormsg = 
	        "No sun synchronous orbit can be found with those parameters, try with " 
		    + GmatStringUtil::Trim(GmatStringUtil::ToString(P_low)) + " < ECC < " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(P_high)) + ", or try " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(i_low)) + " < INC < " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
	  }
	  isError = true;
   }

}

//------------------------------------------------------------------------------
// Real SolveRoAAE(Real RoA, Real i, Real &a, Real &e)
//------------------------------------------------------------------------------
/**
 * Solves for RoAAE, given inputs.
 *
 */
//------------------------------------------------------------------------------
void SunSync::SolveRoAAE(Real RoA, Real i, Real &a, Real &e)
{
   Real diff, tol, aNew, eNew, RoA_low, RoA_high, e_hi, i_high;
   int count;
   Real J2 = 0.0010826267;
   Real omegaSunSync = (2*GmatMathConstants::PI/365.2422)/86400;

   //initialize first run
   e = 0.1;
   a = RoA/(1+e);
   count = 0;
   diff = 1;
   tol = 1e-20;

   while (diff>tol && count<50)
   {
      aNew = 
	     pow(-3*J2*pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*
		 sqrt(GmatSolarSystemDefaults::PLANET_MU[2])*cos(i)/
		 (2*omegaSunSync*pow(1-pow(e,2),2)),2.0/7);
      eNew = (RoA/aNew)-1;
      diff = GmatMathUtil::Abs(a - aNew);
      count++;
      a = aNew;
      e = eNew;
   }

   //if solutions out of range, give upper and lower input bounds 
   //to satisfy requirements
   if (e<0 || count>=50)
   {
      RoA_low = SolveA(0,i,0);
      if (RoA_low == -1 || 
		  RoA_low < (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)) 
	     RoA_low = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100;
      e_hi = 
	     1-(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)/
		 (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000);
      RoA_high = SolveA(e_hi,i,0)*(1+e_hi);
      if (RoA_high < 0 || 
		  (RoA_high > 
		  (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000)*(1+e_hi))) 
	     RoA_high = 
		    (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000)*(1+e_hi);
      i_high = 
		 SolveI(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100, 
		        RoA/(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)-1,0);
	  if (i_high == -1)
	     i_high = 110;
	  if ((RoA>RoA_low)&&(RoA<RoA_high))
	  {
	     if (i<i_high)
		    errormsg = "No sun synchronous orbit can be found";
		 else
			errormsg = "No sun synchronous orbit can be found, try with INC < " + 
		       GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
	  }
	  else if (i<i_high)
	  {
	     errormsg = "No sun synchronous orbit can be found, try " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(RoA_low)) + " < RA < " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(RoA_high));
	  }
	  else
	  {
         errormsg = "No sun synchronous orbit can be found, try " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(RoA_low)) + " < RA < " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(RoA_high)) + " or INC > " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
	  }
	  isError = true;
   }

}

//------------------------------------------------------------------------------
// Real SolveRoPAE(Real RoP, Real i, Real &a, Real &e)
//------------------------------------------------------------------------------
/**
 * Solves for RoPAE, given inputs.
 *
 */
//------------------------------------------------------------------------------
void SunSync::SolveRoPAE(Real RoP, Real i, Real &a, Real &e)
{
   Real diff, tol, aNew, eNew, RoP_high, RoP_low, e_hi, i_high;
   int count;
   Real J2 = 0.0010826267;
   Real omegaSunSync = (2*GmatMathConstants::PI/365.2422)/86400;

   //initialize first run
   e = 0.1;
   a = RoP/(1-e);
   count = 0;
   diff = 1;
   tol = 1e-20;
    
   while (diff>tol && (count<50))
   {
	  Real ex = 2.0/7;
      aNew = 
	     pow((-3*J2*pow(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2],2)*
		 sqrt(GmatSolarSystemDefaults::PLANET_MU[2])*cos(i)/
		 (2*omegaSunSync*pow(1-pow(e,2),2))),ex);
      eNew = 1-(RoP/aNew);
      diff = GmatMathUtil::Abs(a - aNew);
      count++;
      a = aNew;
      e = eNew;
   }
    
   //if solutions out of range, give upper and lower input bounds 
   //to satisfy requirements
   if (e<0 || count>=50)
   {
      RoP_high = SolveA(0,i,0);
      if (RoP_high == -1 || 
		  RoP_high < (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)) 
	     RoP_high = GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100;
      e_hi = 
	     1-(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)/
		 (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+2000);
      RoP_low = SolveA(e_hi,i,0)*(1-e_hi);
      if (RoP_low < (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100)) 
	     RoP_low = 
		    (GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100);
      i_high = 
	     SolveI(GmatSolarSystemDefaults::PLANET_EQUATORIAL_RADIUS[2]+100, 0,0);
      if ((RoP>RoP_low)&&(RoP<RoP_high))
	  {
	     if ((i*180/GmatMathConstants::PI)<i_high)
		    errormsg = "No sun synchronous orbit can be found";
		 else
			errormsg = "No sun synchronous orbit can be found, try with INC > " + 
		       GmatStringUtil::Trim(GmatStringUtil::ToString(i_high)) + "\n";
	  }
	  else if ((i*180/GmatMathConstants::PI)<i_high)
	  {
	     errormsg = "No sun synchronous orbit can be found, try " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(RoP_low)) + " < RP < " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(RoP_high));
	  }
	  else
	  {
         errormsg = "No sun synchronous orbit can be found, try " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(RoP_low)) + " < RP < " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(RoP_high)) + " or INC > " + 
		    GmatStringUtil::Trim(GmatStringUtil::ToString(i_high));
	  }
	  isError = true;
   }
}
