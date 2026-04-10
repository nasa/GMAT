//$Id$
//------------------------------------------------------------------------------
//                           HermiteInterpolator
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under the FDSS II
// contract, Task Order 08
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: Jan 2, 2018
/**
 * Hermite interpolation class
 */
//------------------------------------------------------------------------------

#ifndef HermiteInterpolator_hpp
#define HermiteInterpolator_hpp

#include "Interpolator.hpp"

/**
 * The Hermite interpolator
 *
 * This implementation of Hermite interpolation allows for interpolation schemes
 * that include derivative information.  The derivative data is used element by
 * element.  That means that the vector that is interpolated can have derivative
 * data for some terms and not for others.
 *
 * The current code implements Hermite-Newton interpolation, using divided
 * differences to build a tableau of terms that are then used for interpolation.
 * The current implementation follows the derivation presented at
 *
 *   http://www.personal.psu.edu/jjb23/web/htmls/sl455SP12/ch3/CH03_4B.pdf
 *
 * along with other sources.
 *
 * The current implementation has 2 restrictions that can be removed by further
 * work:
 *
 * 1. It uses for first derivative inputs only, and does not allow for
 *    incorporation of higher order derivatives.  The code has structures in
 *    place to pull in higher order derivatives (see AddDerivative), but does
 *    not use them.
 *
 * 2. The code requires that each point in the buffer of points used for
 *    interpolation have the same number of derivatives (i.e. either no
 *    derivatives, or first derivatives).  M.J.D. Powell, Approximation Theory
 *    and Methods, Ch. 5, shows how to accommodate derivative data at some
 *    points but not all.  This implementation does not currently suppor that
 *    option.
 */
class GMATUTIL_API HermiteInterpolator: public Interpolator
{
public:
   HermiteInterpolator(const std::string &name, Integer dim, Integer points);
   virtual ~HermiteInterpolator();
   HermiteInterpolator(const HermiteInterpolator &hi);
   HermiteInterpolator&    operator=(const HermiteInterpolator &hi);

   virtual Interpolator*   Clone() const;
   virtual void            Clear();

   virtual bool            AddDerivative(const Real ind, const Real *data,
                                 const Integer order = 1);
   virtual bool            Interpolate(const Real ind, Real *results);
   bool                    InterpolateCartesianState(const Real ind, Real *results);

protected:
   /// Number of data points for interpolation at the desired level
   Integer pointsWanted;
   /// Derivative data

   /// Toggle used to go between Newtonian polynomials and Lagrange polynomials
   bool interpolateNewtonian;

   /// Derivative data container (dimension x num points x num derivatives)
   std::vector<std::vector<RealArray>> derivatives;
   /// Hermite polynomial coefficients (dimension x order)
   std::vector<RealArray> qCoeffs;
   /// Independent data used with the polynomials
   std::vector<RealArray> tValues;

//   // Inherited methods that need some revision for HermiteInterpolator
//   virtual void AllocateArrays();
//   virtual void CleanupArrays();
//   virtual void CopyArrays(const HermiteInterpolator &i);
//
   // Newtonian divided difference structures and methods


   bool                    BuildQCoefficients();
   bool                    EvaluatePolynomial(const Real ind, Real *results);
   bool                    EvaluatePolynomialDerivative(const Real ind, Real *results);
   Real                    EvaluateDerivativeIndependentTerm(const Real ind, Integer order, Integer forElement);


//   // Lagrange polynomial structures and methods
//   void EvaluateDerivative(const Integer atPoint, const Integer ofOrder,
//         Real *deriv, const Integer forCount = 3);
//   Real Alpha();
//   Real Beta();
};

#endif /* HermiteInterpolator_hpp */
