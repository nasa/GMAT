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
// Modified: 2003/09/15 Linda Jun - Put functions under namespace GmatRealUtil.
//                      Added NearestInt().
//
/**
 * This file provides measurement conversion constatns and Math Utilities that
 * are not provided in the C++ Library or provides call-throughs to the
 * routines of the C++ (C) math.h
 */
//------------------------------------------------------------------------------
#ifndef RealUtilities_hpp
#define RealUtilities_hpp

#include "BaseException.hpp"
#include "GmatConstants.hpp"
#include "GmatTime.hpp"

struct GMATUTIL_API RealUtilitiesExceptions
{
   class GMATUTIL_API ArgumentError : public BaseException
   {public : ArgumentError(const std::string& message = "")
       : BaseException("", message) {};  };
   
   class GMATUTIL_API IllegalTime   : public BaseException
   {public : IllegalTime(const std::string& message = "")
       : BaseException("", message) {};  };
};

namespace GmatMathUtil
{
   Integer GMATUTIL_API Abs(Integer theNumber);
   Real    GMATUTIL_API Abs(Real theNumber);
   Real    GMATUTIL_API NearestInt(Real theNumber);
   Real    GMATUTIL_API Round(Real theNumber);
   Real    GMATUTIL_API Floor(Real theNumber);
   Real    GMATUTIL_API Fix(Real theNumber);
   Real    GMATUTIL_API Ceiling(Real theNumber);
   Real    GMATUTIL_API Mod(Real left, Real right);
   Real    GMATUTIL_API Rem(Real left, Real right);
   void    GMATUTIL_API Quotient(Real top, Real bottom, Integer &result);
   void    GMATUTIL_API Quotient(Real top, Real bottom, Real &result);
   Real    GMATUTIL_API Min(Real left, Real right);
   Real    GMATUTIL_API Max(Real left, Real right);
   bool    GMATUTIL_API IsPositive(Real theNumber);
   bool    GMATUTIL_API IsNegative(Real theNumber);
   bool    GMATUTIL_API IsNonNegative(Real theNumber);
   bool    GMATUTIL_API IsZero(Real theNumber, Real accuracy=GmatRealConstants::REAL_EPSILON);
   bool    GMATUTIL_API IsEqual(Real left, Real right,
                    Real accuracy = GmatRealConstants::REAL_EPSILON);
   bool    GMATUTIL_API IsEqual(GmatTime left, GmatTime right, 
                    Real accuracy = GmatRealConstants::REAL_EPSILON);

   Integer GMATUTIL_API SignOf(Real theNumber);
   bool    GMATUTIL_API IsOdd(Integer theNumber);
   bool    GMATUTIL_API IsEven(Integer theNumber);
   
   Real    GMATUTIL_API Rad(Real angleInDeg, bool modBy2Pi = false);
   Real    GMATUTIL_API Deg(Real angleInRad, bool modBy360 = false);
   Real    GMATUTIL_API DegToRad(Real deg, bool modBy2Pi = false);
   Real    GMATUTIL_API RadToDeg(Real rad, bool modBy360 = false);
   Real    GMATUTIL_API ArcsecToDeg(Real asec,bool modBy360 = false);
   Real    GMATUTIL_API ArcsecToRad(Real asec,bool modBy2Pi = false);
   
   Real    GMATUTIL_API Sin(Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMATUTIL_API SinXOverX(Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMATUTIL_API Cos(Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMATUTIL_API Tan(Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI);

   Real    GMATUTIL_API Cosh(Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMATUTIL_API Sinh(Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMATUTIL_API Tanh(Real angleInRad, Real cycleInRad=GmatMathConstants::TWO_PI);
   
   Real    GMATUTIL_API ASin(Real x, Real tol=GmatRealConstants::REAL_TOL, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMATUTIL_API ACos(Real x, Real tol=GmatRealConstants::REAL_TOL, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMATUTIL_API ATan(Real y, Real x=1.0, Real cycleInRad=GmatMathConstants::TWO_PI);
   
   Real    GMATUTIL_API ATan2(Real y, Real x=1.0, Real cycleInRad=GmatMathConstants::TWO_PI);
   
   Real    GMATUTIL_API ASinh(Real x, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMATUTIL_API ACosh(Real x, Real cycleInRad=GmatMathConstants::TWO_PI);
   Real    GMATUTIL_API ATanh(Real x, Real cycleInRad=GmatMathConstants::TWO_PI);
   
   Real    GMATUTIL_API Ln(Real x);             // Natural (base e) Logarithm of x
   Real    GMATUTIL_API Log(Real x);            // Natural (base e) Logarithm of x
   Real    GMATUTIL_API Log10(Real x);          // Base 10 Logarithm of x
   Real    GMATUTIL_API Log(Real x, Real base); // Base <base> Logarithm of x
   Real    GMATUTIL_API Log(Real x, Integer base);

   Real    GMATUTIL_API Rand(Real lowerBound = 0.0, Real upperBound = 1.0);
   Real    GMATUTIL_API Randn(Real mean = 0.0, Real stdev = 1.0);
   void    GMATUTIL_API SetSeed(UnsignedInt seed);
   
   // Commented out. These are not used in anywhere. (LOJ: 2016.04.20)
   // void    GMATUTIL_API SetSeed(Integer initialSeed1, Integer initialSeed2);
   // void    GMATUTIL_API GetSeed(Integer& initialSeed1, Integer& initialSeed2);
   // //loj:void    GMATUTIL_API SetSeedByClock();
   // Real    GMATUTIL_API Number(Real lowerBound=0.0, Real upperBound=1.0);
   // Real    GMATUTIL_API GaussianNumber(Real mu= 0.0, Real sigma=1.0);
   // Real    GMATUTIL_API Ran();
   // void    GMATUTIL_API SetRanKey(Real k=0.0);
   
   //loj:Real    GMATUTIL_API Cot(Real x);
   Real    GMATUTIL_API Cbrt(Real x);
   Real    GMATUTIL_API Sqrt(Real x);
   
   Real    GMATUTIL_API Exp(Real x);            // Raises e  to the x power(e^x)
   Real    GMATUTIL_API Exp10(Real x);          // Raises 10 to the x power(10^x)
   Real    GMATUTIL_API Pow(Real x, Real y);    // Raises x to the y power(x^y)
   Real    GMATUTIL_API Pow(Real x, Integer y); // Raises x to the y power(x^y)
   
   bool    GMATUTIL_API IsNaN(Real x);
   bool    GMATUTIL_API IsInf(Real x);
}

// This inline doesn't work with MSVC++ compiler
#ifndef _MSC_VER
#include <cmath>
//------------------------------------------------------------------------------
// bool IsNaN(Real x)
//------------------------------------------------------------------------------
/**
 * Tests if input value is not a number.
 */
//------------------------------------------------------------------------------
inline bool GmatMathUtil::IsNaN(Real x)
{
   return std::isnan(x);
}

//------------------------------------------------------------------------------
// bool IsInf(Real x)
//------------------------------------------------------------------------------
/**
 * Tests if input value is a infinite number.
 */
//------------------------------------------------------------------------------
inline bool GmatMathUtil::IsInf(Real x)
{
   return std::isinf(x);
}
#endif

#endif // RealUtilities_hpp
