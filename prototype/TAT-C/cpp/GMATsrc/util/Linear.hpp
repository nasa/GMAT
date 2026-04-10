//$Id$
//------------------------------------------------------------------------------
//                                 Linear
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
// Author: Matthew Weippert, Chu-chi Li, E. Corderman
// Created: 1995/07/21 for GSS project
// Modified: 2003/09/16 Linda Jun - Fixed deprecated OMANIP.
//
/**
 * Declares Linear Algebra conversion, Linear I/O, and Linear Math operations.
 */
//------------------------------------------------------------------------------
#ifndef Linear_hpp
#define Linear_hpp

#include <iostream>
#include <iomanip>

#include "utildefs.hpp"
#include "Rvector.hpp"
#include "Rvector3.hpp"
#include "Rmatrix.hpp"
#include "Rmatrix33.hpp"

namespace GmatRealUtil
{
   // spherical coordinate types and operations 
   struct RaCodec
   {
      Real radiusD;
      Real rightAscensionD;
      Real coDeclinationD;
   };
   
   struct RaDec
   {
      Real radiusD;
      Real rightAscensionD;
      Real declinationD;
   };
   
   // Conversion Functions for Spherical coordinates
   GMATUTIL_API RaCodec  CartesianToRaCodec(const Rvector3 &r);
   GMATUTIL_API RaDec    CartesianToRaDec(const Rvector3 &r);
   GMATUTIL_API Rvector3 RaCodecToCartesian(const RaCodec &r);
   GMATUTIL_API RaDec    RaCodecToRaDec(const RaCodec &r);
   GMATUTIL_API Rvector3 RaDecToCartesian(const RaDec &r);
   GMATUTIL_API RaCodec  RaDecToRaCodec(const RaDec &r);
   
   // Math Utilities
   GMATUTIL_API Real Min(const Rvector &numbers);
   GMATUTIL_API Real Max(const Rvector &numbers);
   
   //  I/O stream operations
   GMATUTIL_API std::istream& operator>> (std::istream &input, Rvector &a);
   GMATUTIL_API std::ostream& operator<< (std::ostream &output, const Rvector &a);
   GMATUTIL_API std::istream& operator>> (std::istream &input, Rmatrix &a);
   GMATUTIL_API std::ostream& operator<< (std::ostream &output, const Rmatrix &a);
   
   GMATUTIL_API std::string RealToString(const Real &rval, bool useCurrentFormat = true,
      bool scientific = false, bool showPoint = false,
      Integer precision = GmatGlobal::DATA_PRECISION,
      Integer width = GmatGlobal::DATA_WIDTH);

   GMATUTIL_API std::string ToString(const Real &rval, bool useCurrentFormat = true,
                        bool scientific = false, bool showPoint = false,
                        Integer precision = GmatGlobal::DATA_PRECISION,
                        Integer width = GmatGlobal::DATA_WIDTH);
   
   GMATUTIL_API std::string ToString(const Integer &ival, bool useCurrentFormat = true,
                        Integer width = GmatGlobal::INTEGER_WIDTH);
}
#endif // Linear_hpp

