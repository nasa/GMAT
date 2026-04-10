//------------------------------------------------------------------------------
//                         LobattoIIIaMathUtil
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002-2026 United States Government as represented by the
// Administrator of The National Aeronautics and Space Administration.
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
// Author: Youngkwang Kim
// Created: 2015/07/08
//
// NOTE: This is a static utility class.  The constructors, destructor, and
// operator= are not implemented.  No instances of this class can be
// instantiated.
//
/**
* This class has static methods for Implicit RK collocation method
*/
//------------------------------------------------------------------------------
#ifndef LobattoIIIaMathUtil_hpp
#define LobattoIIIaMathUtil_hpp

#include "csaltdefs.hpp"
#include "Rmatrix.hpp"
#include "Rvector.hpp"
#include "SparseMatrixUtil.hpp" // for sparse matrix

#include "LowThrustException.hpp"
#include "MessageInterface.hpp"

//#define DEBUG


/*
* LobattoIIIaMathUtil class
*/
class LobattoIIIaMathUtil
{
public:
   // Conduct Hermite interpolation
   static void    HermiteInterpolation(const Rvector *interpPts, 
                                       const Rvector *timePoints,
                                       const std::vector< Rvector> &stateArray,
                                       const std::vector< Rvector> &dynArray,
                                       std::vector< Rvector> &psdStateVec,
                                       std::vector< Rvector> &psdDynVec);
   // Conduct Hermite interpolation
   static void    HermiteInterpolation(Real time, 
                                       const Rvector *timePoints,
                                       const std::vector< Rvector> &stateArray,
                                       const std::vector< Rvector> &dynArray,
                                       Rvector &psdStateVec,
                                       Rvector &psdDynVec);
// Conduct Hermite interpolation
   static void    HermiteInterpolation(const Rvector *interpPoints,
                                       const Rvector *dataPoints,
                                       const std::vector< Rvector> &stateArray,
                                       const std::vector< Rvector> &dynArray,
                                       Rmatrix &psdStateVec,
                                       Rmatrix &psdDynVec);
   // Compute function values based on polynomical coefficients
   static Real    ComputeFunctionValue(Real time, Rvector coeff);
   // Compute derivative values based on polynomical coefficients
   static Real    ComputeDerivativeValue(Real time, Rvector coeff);
   // Compute polynomical coefficients based on function and derivative values
   static Rvector GetHermiteCoeff(const Rvector *timeVec,
                                  const Rvector &funcValues,
                                  const Rvector &dynValues);   
   
private:
   /// private constructors, destructor, operator=   *** UNIMPLEMENTED ***
   LobattoIIIaMathUtil();
   LobattoIIIaMathUtil(const LobattoIIIaMathUtil &copy);
   LobattoIIIaMathUtil& operator=(const LobattoIIIaMathUtil &copy);
   virtual ~LobattoIIIaMathUtil();
   // conduct convolution. Used by GetHermiteCoeff
   static Rvector Convolution(Rvector x, Rvector h);
};

#endif // LobattoIIIaMathUtil_hpp
