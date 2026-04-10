//$Id$
//------------------------------------------------------------------------------
//                              RealUtilities
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
//
// Copyright (c) 2002 - 2018 United States Government as represented by the
// Administrator of the National Aeronautics and Space Administration.
// All Other Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License"); 
// You may not use this file except in compliance with the License. 
// You may obtain a copy of the License at:
// http://www.apache.org/licenses/LICENSE-2.0. 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either 
// express or implied.   See the License for the specific language
// governing permissions and limitations under the License.
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Created: 1995/07/21 for GSS project
// Modified:
//   2003/09/15 Linda Jun - Used namespace GmatRealUtil
//
/**
 *  This file provides Math Utilities that are not provided in the C++ Library 
 *  or provides call-throughs to the routines of the C++ (C) math.h
 */
//------------------------------------------------------------------------------
#include <math.h>             // for fabs()
#include "RealUtilities.hpp"
#include "GmatConstants.hpp"
#include "Linear.hpp"         // for GmatRealUtil::
#include "RandomNumber.hpp"   // for Uniform() and Gaussian()

//#define DEBUG_MATH_UTIL

#ifdef DEBUG_MATH_UTIL
#include "MessageInterface.hpp"
#endif

using namespace GmatMathUtil;
using namespace GmatMathConstants;

//------------------------------------------------------------------------------
//  Integer Abs(Integer theNumber)
//------------------------------------------------------------------------------
Integer GmatMathUtil::Abs(Integer theNumber)
{
//    return abs(theNumber);   // cannot find abs prototype?  temporarily ...
   if (theNumber < 0)
   {
      return (-theNumber);
   }
   return theNumber;
}

//------------------------------------------------------------------------------
//  Real Abs(Real theNumber)
//------------------------------------------------------------------------------
Real GmatMathUtil::Abs(Real theNumber)
{
   return fabs(theNumber);
}

//------------------------------------------------------------------------------
//  Real NearestInt(Real theNumber)
//------------------------------------------------------------------------------
Real GmatMathUtil::NearestInt(Real theNumber)
{
   Real retVal;

   if (theNumber < 0.0)
      retVal = ceil(theNumber - 0.5);
   else
      retVal = floor(theNumber + 0.5);
   
   return retVal;
}

//------------------------------------------------------------------------------
//  Real Round(Real theNumber)
//------------------------------------------------------------------------------
Real GmatMathUtil::Round(Real theNumber)
{
   return floor(theNumber + 0.5 );
}

//------------------------------------------------------------------------------
//  Real Floor(Real theNumber)
//------------------------------------------------------------------------------
Real GmatMathUtil::Floor(Real theNumber)
{
   return floor(theNumber);
}

//------------------------------------------------------------------------------
//  Real Fix(Real theNumber)
//------------------------------------------------------------------------------
Real GmatMathUtil::Fix(Real theNumber)
{
   Real towardZero = SignOf(theNumber) * Floor(Abs(theNumber));
   return towardZero;
}

//------------------------------------------------------------------------------
//  Real Ceiling(Real theNumber)
//------------------------------------------------------------------------------
Real GmatMathUtil::Ceiling(Real theNumber)
{
   return ceil(theNumber);
}

//------------------------------------------------------------------------------
//  Real Mod (Real left, Real right)
//------------------------------------------------------------------------------
Real GmatMathUtil::Mod (Real left, Real right)
{
   if (right == 0.0)  
   {
      throw RealUtilitiesExceptions::ArgumentError("Mod(x, 0.0)");
   };
   return (left - Real(floor(left/right)*right));
}

//------------------------------------------------------------------------------
//  Real Rem (Real left, Real right)
//------------------------------------------------------------------------------
Real GmatMathUtil::Rem (Real left, Real right)
{
   if (right == 0.0) 
   {
      throw RealUtilitiesExceptions::ArgumentError("Rem(x, 0.0)");
   }; 
   return((left-Real(Integer(left/right))*right));
}

//------------------------------------------------------------------------------
//  void Quotient (Real top, Real bottom, Integer &result)
//------------------------------------------------------------------------------
void GmatMathUtil::Quotient (Real top, Real bottom, Integer &result)
{ 
    if (bottom == 0.0) 
    {
        throw RealUtilitiesExceptions::ArgumentError("Quotient(x, 0.0, Integer)");
    }; 
    result=Integer(top/bottom);
}

//------------------------------------------------------------------------------
//  void Quotient (Real top, Real bottom, Real &result)
//------------------------------------------------------------------------------
void GmatMathUtil::Quotient (Real top, Real bottom, Real &result)
{
   if (bottom == 0.0) 
   {
      throw RealUtilitiesExceptions::ArgumentError("Quotient(x, 0.0, Real)");
   }; 

   result= Real(Integer(top/bottom));
}

//------------------------------------------------------------------------------
//  Real Min (Real left, Real right)
//------------------------------------------------------------------------------
Real GmatMathUtil::Min (Real left, Real right)
{
   return ( (left > right) ? right : left );
}

//------------------------------------------------------------------------------
//  Real Max (Real left, Real right)    
//------------------------------------------------------------------------------
Real GmatMathUtil::Max (Real left, Real right)    
{
   return ( (left > right) ? left : right );
}

//------------------------------------------------------------------------------
//  bool IsPositive(Real theNumber)
//------------------------------------------------------------------------------
bool GmatMathUtil::IsPositive(Real theNumber)
{
   return (bool(theNumber > 0.0));
}

//------------------------------------------------------------------------------
//  bool IsNegative(Real theNumber)
//------------------------------------------------------------------------------
bool GmatMathUtil::IsNegative(Real theNumber)
{
   return bool(theNumber < 0.0);
}

//------------------------------------------------------------------------------
//  bool IsNonNegative(Real theNumber)
//------------------------------------------------------------------------------
bool GmatMathUtil::IsNonNegative(Real theNumber)
{
   return bool(theNumber >= 0.0);
}

//------------------------------------------------------------------------------
//  bool IsZero (Real theNumber, Real accuracy)
//------------------------------------------------------------------------------
bool GmatMathUtil::IsZero (Real theNumber, Real accuracy)
{
   return bool(fabs(theNumber) < accuracy);
}


//------------------------------------------------------------------------------
//  bool IsEqual (Real left, Real right, Real accuracy)
//------------------------------------------------------------------------------
bool GmatMathUtil::IsEqual (Real left, Real right, Real accuracy)
{
   return bool(fabs(left-right) < accuracy);
}

//------------------------------------------------------------------------------
//  Integer SignOf (Real theNumber)
//------------------------------------------------------------------------------
Integer GmatMathUtil::SignOf (Real theNumber)
{
   if (IsZero(theNumber)) 
      return 0;
   else if (GmatMathUtil::IsPositive(theNumber)) 
      return 1;

   return -1;
}

//------------------------------------------------------------------------------
//  bool IsOdd(Integer theNumber)
//------------------------------------------------------------------------------
bool GmatMathUtil::IsOdd(Integer theNumber)   // MSVC doesn't like %?
{
   Integer byTwo = theNumber / 2;
   if ((theNumber - (byTwo * 2)) == 0)  return false;
   return true;
}

//------------------------------------------------------------------------------
//  bool IsEven(Integer theNumber)
//------------------------------------------------------------------------------
bool GmatMathUtil::IsEven(Integer theNumber)
{
   Integer byTwo = theNumber / 2;
   if ((theNumber - (byTwo * 2)) == 0)  return true;
   return false;
}


//------------------------------------------------------------------------------
//  Real Rad (Real angleInDeg, bool modBy2Pi)
//------------------------------------------------------------------------------
Real GmatMathUtil::Rad (Real angleInDeg, bool modBy2Pi)
{
   Real x = RAD_PER_DEG * angleInDeg;

   return (modBy2Pi ? x - floor(x/TWO_PI)*TWO_PI
           : x);
}

//------------------------------------------------------------------------------
//  Real Deg(Real angleInRad, bool modBy360)
//------------------------------------------------------------------------------
Real GmatMathUtil::Deg(Real angleInRad, bool modBy360)
{
   Real x = DEG_PER_RAD * angleInRad;

   return (modBy360 ? x - floor(x/360.0)*360.0 : x);
}

//------------------------------------------------------------------------------
//  Real DegToRad(Real deg, bool modBy2Pi = false)
//------------------------------------------------------------------------------
Real GmatMathUtil::DegToRad(Real deg, bool modBy2Pi)
{
   return Rad(deg, modBy2Pi);
}

//------------------------------------------------------------------------------
//  Real RadToDeg(Real rad, bool modBy360 = false)
//------------------------------------------------------------------------------
Real GmatMathUtil::RadToDeg(Real rad, bool modBy360)
{
   return Deg(rad, modBy360);
}

//------------------------------------------------------------------------------
//  Real ArcsecToDeg(Real asec, bool modBy360 = false)
//------------------------------------------------------------------------------
Real GmatMathUtil::ArcsecToDeg(Real asec, bool modBy360)
{
   Real x= DEG_PER_ARCSEC * asec;
   
   return (modBy360 ? x - floor(x/360.0)*360.0 : x);
}

//------------------------------------------------------------------------------
//  Real ArcsecToRad (Real asec, bool modBy2Pi)
//------------------------------------------------------------------------------
Real GmatMathUtil::ArcsecToRad (Real asec, bool modBy2Pi)
{
   Real x = RAD_PER_ARCSEC * asec;
   
   return (modBy2Pi ? x - floor(x/TWO_PI)*TWO_PI
                    : x);
}


//------------------------------------------------------------------------------
//  Real Sin (Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI)
//------------------------------------------------------------------------------
Real GmatMathUtil::Sin (Real angleInRad, Real cycleInRad)
{
   if (cycleInRad<=0.0) 
      throw RealUtilitiesExceptions::ArgumentError("Sin(angle, cycle <= 0.0)");

   return sin( (TWO_PI/cycleInRad)*angleInRad);
}


//------------------------------------------------------------------------------
//  Real SinXOverX (Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI)
//------------------------------------------------------------------------------
Real GmatMathUtil::SinXOverX (Real angleInRad, Real cycleInRad)
{
   if ( Abs(angleInRad) > Pow(10.0,(-(GmatRealConstants::REAL_DIG/2)))) 
   {
      Real sinValue = GmatMathUtil::Sin(angleInRad, cycleInRad);
      return sinValue/angleInRad;
   }
   return 1.0;
}


//------------------------------------------------------------------------------
//  Real Cos (Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI)
//------------------------------------------------------------------------------
Real GmatMathUtil::Cos (Real angleInRad, Real cycleInRad)
{
   if (cycleInRad<=0.0) 
      throw RealUtilitiesExceptions::ArgumentError("Cos(angle, cycle <= 0.0)\n");
   
   return cos( (TWO_PI/cycleInRad)*angleInRad);
}

//------------------------------------------------------------------------------
//  Real Tan (Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI)
//------------------------------------------------------------------------------
Real GmatMathUtil::Tan (Real angleInRad, Real cycleInRad)
{
   if (cycleInRad <= 0.0) 
      throw RealUtilitiesExceptions::ArgumentError("Tan(angle, cycle <= 0.0)\n");

   return tan( (TWO_PI/cycleInRad)*angleInRad);
}

//------------------------------------------------------------------------------
// Real Cosh (Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI)
//------------------------------------------------------------------------------
Real GmatMathUtil::Cosh (Real angleInRad, Real cycleInRad)
{
   if (cycleInRad <= 0.0) 
      throw RealUtilitiesExceptions::ArgumentError("Cosh(angle, cycle <= 0.0)\n");
   else if (IsEqual(GmatMathUtil::Cos(angleInRad,cycleInRad),0.0)) 
      throw RealUtilitiesExceptions::ArgumentError();
   
   return cosh( (TWO_PI/cycleInRad)*angleInRad);
}

//------------------------------------------------------------------------------
// Real Sinh (Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI)
//------------------------------------------------------------------------------
Real GmatMathUtil::Sinh (Real angleInRad, Real cycleInRad)
{
   if (cycleInRad <= 0.0) 
      throw RealUtilitiesExceptions::ArgumentError("Sinh(angle, cycle <= 0.0)\n");
   else if (IsEqual(GmatMathUtil::Cos(angleInRad,cycleInRad),0.0)) 
      throw RealUtilitiesExceptions::ArgumentError();
   
   return sinh( (TWO_PI/cycleInRad)*angleInRad);
}

//------------------------------------------------------------------------------
// Real Tanh (Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI)
//------------------------------------------------------------------------------
Real GmatMathUtil::Tanh (Real angleInRad, Real cycleInRad)
{
   if (cycleInRad <= 0.0) 
      throw RealUtilitiesExceptions::ArgumentError("Tanh(angle, cycle <= 0.0)\n");
   else if (IsEqual(GmatMathUtil::Cos(angleInRad,cycleInRad),0.0)) 
      throw RealUtilitiesExceptions::ArgumentError();
   
   return tanh( (TWO_PI/cycleInRad)*angleInRad);
}

//------------------------------------------------------------------------------
//  Real ASin (Real x, Real tol=GmatRealConstants::REAL_TOL,
//             Real cycleInRad=GmatMathConstants::TWO_PI)
//------------------------------------------------------------------------------
Real GmatMathUtil::ASin (Real x, Real tol, Real cycleInRad)
{
   if (cycleInRad <= 0.0) 
      throw RealUtilitiesExceptions::ArgumentError("ASin(angle, cycle <= 0.0)\n");
//   else if ((fabs(x) - 1.0) > GmatRealConstants::REAL_TOL)
//      throw RealUtilitiesExceptions::ArgumentError("ASin(value > 1.0, cycle)\n");
   if ((fabs(x) - 1.0) > 0.0)
   {
      #ifdef DEBUG_MATH_UTIL
      MessageInterface::ShowMessage
         ("ACos() checking additional tolerance for x=%.16f\n", x);
      #endif

      if ((x > 1.0 - tol) && (x <= 1.0 + tol))
         return PI_OVER_TWO;
      else if ((x > -1.0 - tol) && (x <= -1.0 + tol))
         return - PI_OVER_TWO;
      else
         throw RealUtilitiesExceptions::ArgumentError
            ("The input \"" + GmatRealUtil::ToString(x, false, false, true, 15, 1) +
             "\" to ASin() is not within -1.0 and 1.0.");
   }
   
   return (cycleInRad/TWO_PI)*asin(x);
}


//------------------------------------------------------------------------------
//  Real ACos (Real x, Real tol=GmatRealConstants::REAL_TOL,
//             Real cycleInRad=GmatMathConstants::TWO_PI)
//------------------------------------------------------------------------------
Real GmatMathUtil::ACos (Real x, Real tol, Real cycleInRad)
{
   if (cycleInRad <= 0.0)
      throw RealUtilitiesExceptions::ArgumentError("ACos(angle, cycle <= 0.0)\n");
   
   if ((fabs(x) - 1.0) > 0.0)
   {
      #ifdef DEBUG_MATH_UTIL
      MessageInterface::ShowMessage
         ("ACos() checking additional tolerance for x=%.16f\n", x);
      #endif
      
      if ((x > 1.0 - tol) && (x <= 1.0 + tol))
         return 0.0;
      else if ((x > -1.0 - tol) && (x <= -1.0 + tol))
         return PI;
      else
         throw RealUtilitiesExceptions::ArgumentError
            ("The input \"" + GmatRealUtil::ToString(x, false, false, false, 15, 1) +
             "\" to ACos() is not within -1.0 and 1.0.");
   }
   
   return (cycleInRad/TWO_PI)*acos(x);
}


//------------------------------------------------------------------------------
//  Real ATan (Real y, Real x , Real cycleInRad=GmatMathConstants::TWO_PI)
//------------------------------------------------------------------------------
Real GmatMathUtil::ATan (Real y, Real x , Real cycleInRad)
{
   if (cycleInRad <= 0.0) 
      throw RealUtilitiesExceptions::ArgumentError("ATan(angle, cycle <= 0.0)\n");
   
   return (cycleInRad/TWO_PI)*atan2(y,x);
}


//------------------------------------------------------------------------------
//  Real ATan2 (Real y, Real x , Real cycleInRad=GmatMathConstants::TWO_PI)
//------------------------------------------------------------------------------
Real GmatMathUtil::ATan2 (Real y, Real x , Real cycleInRad)
{
   if (cycleInRad <= 0.0) 
      throw RealUtilitiesExceptions::ArgumentError("ATan(angle, cycle <= 0.0)\n");
   
   return (cycleInRad/TWO_PI)*atan2(y,x);
}


//------------------------------------------------------------------------------
//  Real ASinh (Real x, Real cycleInRad=GmatMathConstants::TWO_PI)
//------------------------------------------------------------------------------
Real GmatMathUtil::ASinh (Real x, Real cycleInRad)
{
   if (cycleInRad <= 0.0) 
      throw RealUtilitiesExceptions::ArgumentError("ASinh(angle, cycle <= 0.0)\n");
   
#ifndef _MSC_VER  // if not Microsoft Visual C++
   return (cycleInRad/TWO_PI)*asinh(x);
#else
   Real asinh = 0.0;
	if (x > 0)
		asinh =  log(x + sqrt(x * x + 1));
	else
		asinh = -log(-x + sqrt(x * x + 1));
   
   return (cycleInRad/TWO_PI) * asinh;
#endif
}


//------------------------------------------------------------------------------
//  Real ACosh (Real x, Real cycleInRad=GmatMathConstants::TWO_PI)
//------------------------------------------------------------------------------
Real GmatMathUtil::ACosh (Real x, Real cycleInRad)
{
   if (cycleInRad <= 0.0) 
      throw RealUtilitiesExceptions::ArgumentError
         ("Invalid cycle in radians " + GmatRealUtil::ToString(cycleInRad, false, false, false, 15, 1) +
          " entered, ACosh is undefined for input cycle value <= 0\n");
   
#ifndef _MSC_VER  // if not Microsoft Visual C++
   return (cycleInRad/TWO_PI)*acosh(x);
#else
   Real acosh = 0.0;
	if (x >= 1.0)
		acosh =  log(x + sqrt(x * x - 1));
	else
		throw RealUtilitiesExceptions::ArgumentError
         ("Invalid input " + GmatRealUtil::ToString(x, false, false, false, 15, 1) +
          " entered, ACosh is undefined for input values < 1.0\n");
   
   return (cycleInRad/TWO_PI) * acosh;
   
#endif
}


//------------------------------------------------------------------------------
//  Real ATanh (Real x, Real cycleInRad=GmatMathConstants::TWO_PI)
//------------------------------------------------------------------------------
/**
 * Computes inverse hyperbolic tangent of x.
 */
//------------------------------------------------------------------------------
Real GmatMathUtil::ATanh (Real x, Real cycleInRad)
{
   if (cycleInRad <= 0.0) 
      throw RealUtilitiesExceptions::ArgumentError("ATanh(angle, cycle <= 0.0)\n");
   
   //VC++ error C3861: 'atanh': identifier not found
#ifndef _MSC_VER  // if not Microsoft Visual C++
   return (cycleInRad/TWO_PI) * atanh(x);
#else
   Real atanh = (0.5) * Log((1.0+x)/(1.0-x));
   return (cycleInRad/TWO_PI) * atanh;
#endif
}


//------------------------------------------------------------------------------
//  Real Ln(Real x)
//------------------------------------------------------------------------------
/**
 * @return Natural logarithm for x
 */
//------------------------------------------------------------------------------
Real GmatMathUtil::Ln(Real x)
{
   if (x <= 0.0)
      throw RealUtilitiesExceptions::ArgumentError("Ln(x <= 0.0)\n");

   return (log(x));
}


//------------------------------------------------------------------------------
//  Real Log(Real x)
//------------------------------------------------------------------------------
/**
 * @return Natural logarithm for x
 */
//------------------------------------------------------------------------------
Real GmatMathUtil::Log(Real x)
{
   if (x <= 0.0)
      throw RealUtilitiesExceptions::ArgumentError("Log(x <= 0.0)\n");

   return (log(x));
}


//------------------------------------------------------------------------------
//  Real Log10(Real x)
//------------------------------------------------------------------------------
/**
 * @return Base 10 logarithm for x
 */
//------------------------------------------------------------------------------
Real GmatMathUtil::Log10(Real x)
{
   if (x <= 0.0)
      throw RealUtilitiesExceptions::ArgumentError("Log10(x <= 0.0)\n");

   return (log10(x));
}


//------------------------------------------------------------------------------
//  Real Log(Real x, Real base)
//------------------------------------------------------------------------------
/**
 * @return Base <base> Logarithm of x
 */
//------------------------------------------------------------------------------
Real GmatMathUtil::Log(Real x, Real base)
{
   if ( (x <= 0.0) || (base <= 0.0) ) 
      throw RealUtilitiesExceptions::ArgumentError();

   return (log10(x) / log10(base));
}


//------------------------------------------------------------------------------
//  Real Log (Real x, Integer base)
//------------------------------------------------------------------------------
/**
 * @return Base <base> Logarithm of x
 */
//------------------------------------------------------------------------------
Real GmatMathUtil::Log (Real x, Integer base)
{
   return GmatMathUtil::Log(x, Real(base));
}

//------------------------------------------------------------------------------
// Real Rand(Real lowerBound=0.0, Real upperBound=1.0)
//------------------------------------------------------------------------------
/**
 * Returns uniformly distributed pseudorandom number
 */
//------------------------------------------------------------------------------
Real GmatMathUtil::Rand(Real lowerBound, Real upperBound)
{
   RandomNumber* rn = RandomNumber::Instance();
   return rn->Uniform(lowerBound, upperBound);
}

//------------------------------------------------------------------------------
// Real Randn(Real mean, Real stdev)
//------------------------------------------------------------------------------
/**
 * Returns normally distributed pseudorandom numbers.
 * Actually the normal distribution is the sub form of Gaussian distribution.
 * Gaussian distribution have 2 parameters, mean and variance. When there is
 * zero mean and unit variance the Gaussian distribution becomes normal
 * other wise it is pronounced as Gaussian.
 */
//------------------------------------------------------------------------------
Real GmatMathUtil::Randn(Real mean, Real stdev)
{
   RandomNumber* rn = RandomNumber::Instance();
   return rn->Gaussian(mean, stdev);
}

//------------------------------------------------------------------------------
// void SetSeed(UnsignedInt seed)
//------------------------------------------------------------------------------
void GmatMathUtil::SetSeed(UnsignedInt seed)
{
   RandomNumber* rn = RandomNumber::Instance();
   rn->SetSeed(seed);
}


// Commented out. These are not used in anywhere. (LOJ: 2016.04.20)
// static Integer Seed[2]= {30001,15001};

// //------------------------------------------------------------------------------
// //  void SetSeed(Integer initialSeed1,
// //------------------------------------------------------------------------------
// /**
//  * @note 30_031 = 2*3*5*7*11*13+1
//  *       67_831 = 2*3*5*7*17*19+1;
//  */
// //------------------------------------------------------------------------------
// void GmatMathUtil::SetSeed(Integer initialSeed1, Integer initialSeed2) 
// {
//    Seed[0] = initialSeed1 % 67831;
//    Seed[1] = initialSeed2 % 30031;
// }

// //------------------------------------------------------------------------------
// //  void GetSeed(Integer& initialSeed1, Integer& initialSeed2)
// //------------------------------------------------------------------------------
// void GmatMathUtil::GetSeed(Integer& initialSeed1, Integer& initialSeed2)
// {
//    initialSeed1 = Seed[0];
//    initialSeed2 = Seed[1];
// }

//------------------------------------------------------------------------------
//  void SetSeedByClock() 
//------------------------------------------------------------------------------
//void  GmatMathUtil::SetSeedByClock() 
//{
//  struct timeval tv;
//  struct timezone tz;
//
//  if (gettimeofday(&tv,&tz)==0) 
//  {
//
//    Seed[0] = (Integer) (643*tv.tv_usec) % 67831;
//    Seed[1] = (Integer) (481* tv.tv_usec) % 30031;
//  } 
//  else 
//  {
//    throw RealUtilitiesExceptions::IllegalTime();
//  };
//
//};

// //------------------------------------------------------------------------------
// //  Real Number (Real lowerBound, Real upperBound)
// //------------------------------------------------------------------------------
// /**
//  * @note 211 = 2*3*5*7+1
//  *       2311 = 2*3*5*7*11 + 1
//  *       30031 = 2*3*5*7*11*13+1
//  *       67831 = 2*3*5*7*17*19+1
//  *       2037032760 = (30031 * 67831) -1
//  */
// //------------------------------------------------------------------------------
// Real GmatMathUtil::Number (Real lowerBound, Real upperBound)
// {
//    Real X,Y;

//    Seed[1] = (211 * Seed[1]+2311) % 30031;

//    Seed[0] = 30031* Seed[0] +Seed[1];
//    X = Real(Seed[0] % 2037032760) / Real(2037032760);
   
//    Seed[0] = Seed[0] % 67831;

//    Y = (upperBound-lowerBound)*X + lowerBound;
    
//    return Y;
// }

// //------------------------------------------------------------------------------
// //  Real GaussianNumber(Real mu, Real sigma)
// //------------------------------------------------------------------------------
// Real GmatMathUtil::GaussianNumber(Real mu, Real sigma)
// {
//    Integer i=0;
//    Real Z = -6.0;
   
//    for (i=0;i<12;i++) 
//       Z = Z+Number();
   
//    return mu+Z*sigma;
// }

// struct key
// {
// static Integer x;
// static Integer y;
// static Integer z;
// };

// Integer key::x = 10001;
// Integer key::y = 20001;
// Integer key::z = 30001;

// //------------------------------------------------------------------------------
// //  Real Ran () 
// //------------------------------------------------------------------------------
// Real GmatMathUtil::Ran () 
// {
//    Real w;
//    key::x = 171 * (key::x%177-177) - 2 * (key::x/177);
//    if (key::x<0) 
//    {
//       key::x = key::x + 30269;
//    };
   
//    key::y = 172 * (key::y%176-176)-35 * (key::y/176);
//    if (key::y<0) 
//    {
//       key::y = key::y + 30307;
//    };
   
//    key::z = 170 * (key::z%178-178) -63* (key::z/178);
//    if (key::z<0) 
//    {
//       key::z = key::z +30323;
//    };

//    w = Real(key::x)/30269.0 + Real(key::y)/30307.0 + Real(key::z)/30323.0;
   
//    return (w - Real(Integer(w-0.5)));  
// }

// //------------------------------------------------------------------------------
// //  void SetRanKey(Real k) 
// //------------------------------------------------------------------------------
// void GmatMathUtil::SetRanKey(Real k) 
// {
//    //Real dummy = 0.0;
//    if (k==0) 
//    {
//       key::x = 10001;
//       key::y = 20001;
//       key::z = 30001;
//    } 
//    else 
//    {
//       Integer i;
//       Integer loop = Integer(k-Real(Integer(fabs(k)/31270.0)*31270.0))%31270;
      
//       for (i=1; i<=loop;i++) 
//          //dummy =
//          GmatMathUtil::Ran();
//    }
// }

//------------------------------------------------------------------------------
//  R(Real g, Real f)
//
//  Description: function needed for Cot that produces the actual
//               result value
//  Arguments:                                                                
//  <g>     <-  first input value
//  <f>     <-  second input value
//
//  Returns:  <Real>  magnitude of cot
//---------------------------------------------------------------------------
//  Real GmatMathUtil::R (Real g, Real f) 
//  {
//     if (GmatRealConstants::REAL_DIG<11) 
//     {
//        const Real p0 =1.0;
//        const Real p1 = -0.1113614403566;
//        const Real p2 = 0.1075154738488e-2;
//        const Real q0 = 1.0;
//        const Real q1 = -0.4446947720281;
//        const Real q2 = 0.15973392133003-1;
//        return ((p2*g+p1)*g*f+f)/
//           (((q2*g+q1)*g+0.5)+0.5);
//     } 
//     else 
//     {
//        const Real p0 = 1.0;
//        const Real p1 = -0.13338350006421960681e+0;
//        const Real p2 = 0.34248878235890589960e-2;
//        const Real p3 = -0.17861707342254426711e-4;
//        const Real q0 = 1.0;
//        const Real q1 = -0.46671683339755294240e+0;
//        const Real q2 = 0.25663832289440112864e-1;
//        const Real q3 = -0.31181531907010027307e-3;
//        const Real q4 = 0.49819433993786512270e-6;
//        return (((p3*g+p2)*g+p1)*g*f+f)/
//           (((((q4*g+q3) *g+ q2) * g+ q1) *g +0.5) +0.5);
//     }

//  }

//------------------------------------------------------------------------------
//  Real Cot(Real x) 
//------------------------------------------------------------------------------
//  Real GmatMathUtil::Cot(Real x) 
//  {
//     static Real Epsilon = pow(GmatRealConstants::SHORT_REAL_RADIX, 
//                               -(GmatRealConstants::REAL_MANT_DIG/2));
//     static Real YMax = Real(Integer(PI *
//                                     pow(2, (GmatRealConstants::REAL_MANT_DIG/2))));
//     Real y;
//     Integer n;
//     Real    xn;
//     Real    f,g,x1,x2;
//     Real    result;
//     const Real epsilon1 = 1/GmatRealConstants::REAL_MAX;
//     const Real c1 = 1.57080078125;
//     const Real c2 = -0.4454455103380768678308e-5;

//     y = fabs(x);
    
//     if (y<epsilon1) 
//        throw RealUtilitiesExceptions::ArgumentError();

//     if (y>YMax/2) 
//        throw RealUtilitiesExceptions::ArgumentError();

//     n  = Integer(x*2/PI);
//     xn = n;
//     x1 = Integer(x);
//     x2 = x-x1;

//     f = ((x1-xn*c1) + x2)-xn*c2;

//     if (fabs(f) < Epsilon) 
//     { 
//        result = f;
//     } 
//     else 
//     {
//        g = f*f;
//        result = R(g,f);
//     }
    
//     if (n%2) 
//        return -result;

//     return 1/result;
//  }

//------------------------------------------------------------------------------
//  Real Cbrt(Real x) 
//------------------------------------------------------------------------------
Real GmatMathUtil::Cbrt(Real x) 
{
   Real result;
   
   if (IsZero(x)) 
   {
      result = 0.0;
   } 
   else 
   {
      Real F,Y;
      Integer M,N;
      const Real Cbrt_C1  = 0.5874009;
      const Real Cbrt_C2  = 0.4125990;
      const Real Cbrt_C3  = 0.6299605249;
      const Real Cbrt_C4  = 0.7937005260;

      F=frexp(x,&N);
      F=fabs(F);
      Y= Cbrt_C1 + Cbrt_C2*F;
      
      switch(N%3) 
      {
      case 0:
         break;
      case 1: 
         Y = Cbrt_C3*Y;
         F = F /2;
         N = N+2;
         break;
      case 2:    
         Y = Cbrt_C4*Y;
         F = F/2;
         N = N+1;
      }
 
      M = N/3;
      Y = Y - (Y/3 -((F/3)/(Y*Y)));
      Y = Y - (Y/3 -((F/3)/(Y*Y)));
      Y = Y - (Y/3 -((F/3)/(Y*Y)));
      Y = Y - (Y/3 -((F/3)/(Y*Y)));

      if (GmatMathUtil::IsNegative(x)) 
         Y = -Y;

      result = ldexp(Y,M);
   }
    
   return result;
}


//------------------------------------------------------------------------------
//  Real Sqrt(Real x) 
//------------------------------------------------------------------------------
Real GmatMathUtil::Sqrt(Real x) 
{
   if (x < 0.0)
   {      
      throw RealUtilitiesExceptions::ArgumentError
         ("Sqrt(x) x is less than 0.0.  x = " + GmatRealUtil::ToString(x) +"\n");
   }
   
   return sqrt(x);
}


//------------------------------------------------------------------------------
//  Real Exp(Real x)
//------------------------------------------------------------------------------
/**
 * @return e raised to the x power
 */
//------------------------------------------------------------------------------
Real GmatMathUtil::Exp(Real x)
{
   return exp(x);
}


//------------------------------------------------------------------------------
//  Real Exp10(Real x)
//------------------------------------------------------------------------------
/**
 * @return 10 raised to the x power
 */
//------------------------------------------------------------------------------
Real GmatMathUtil::Exp10(Real x)
{
   return pow(Real(10.0), x);
}


//------------------------------------------------------------------------------
//  Real Pow(Real x, Real y)
//------------------------------------------------------------------------------
/**
 * @return x raised to the y power
 */
//------------------------------------------------------------------------------
Real GmatMathUtil::Pow(Real x, Real y)
{
   return pow(x, y);
}


//------------------------------------------------------------------------------
//  Real Pow(Real x, Integer y)
//------------------------------------------------------------------------------
/**
 * @return x raised to the y power
 */
//------------------------------------------------------------------------------
Real GmatMathUtil::Pow(Real x, Integer y)
{
   return pow(x, y);
}

#ifdef _MSC_VER
#include <float.h>  // for _isnan() on VC++
#define isnan(x) _isnan(x)    // VC++ uses _isnan()  instead of isnan()
#define isinf(x) !_finite(x)  // VC++ uses _finite() instead of isinf()

//------------------------------------------------------------------------------
// bool IsNaN(Real x)
//------------------------------------------------------------------------------
/**
 * Tests if input value is not a number.
 */
//------------------------------------------------------------------------------
bool GmatMathUtil::IsNaN(Real x)
{
   #ifdef _MSC_VER
      return (isnan(x) == 0 ? false : true);
   #else
      return isnan(x);
   #endif
}

//------------------------------------------------------------------------------
// bool IsInf(Real x)
//------------------------------------------------------------------------------
/**
 * Tests if input value is a infinite number.
 */
//------------------------------------------------------------------------------
bool GmatMathUtil::IsInf(Real x)
{
   return isinf(x);
}
#endif


//------------------------------------------------------------------------------
//  bool IsEqual (Real left, Real right, Real accuracy)
//------------------------------------------------------------------------------
bool GmatMathUtil::IsEqual(GmatTime left, GmatTime right, Real accuracy)
{
   return bool(fabs((left - right).GetMjd()) < accuracy);
}

