//------------------------------------------------------------------------------
//                              CubicSpline
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool
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
// Author: Wendy Shoan / NASA
// Created: 2019.04.03
//
/**
 * Original Python prototype:
 * Author: N. Hatten
 *
 * This class is a singleton.  All code that uses it will use the one instance
 * of this class to access its methods.
 * This code was converted from the original python prototype, and includes 
 * original commentary.
 */
//------------------------------------------------------------------------------

#include "CubicSpline.hpp"
#include "UtilityException.hpp"

#include "MessageInterface.hpp"

//#define DEBUG_CUBIC_SPLINE
//#define DEBUG_ARRAYS

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

CubicSpline *CubicSpline::theCubicSpline = NULL;


//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//---------------------------------------------------------------------------
//  CubicSpline* Instance()
//---------------------------------------------------------------------------
/**
 * Instance method for the singleton class.
 *
 * @return Pointer to the Instance of this singleton class
 */
//---------------------------------------------------------------------------
CubicSpline* CubicSpline::Instance()
{
   if (theCubicSpline == NULL)
      theCubicSpline = new CubicSpline();
   
   return theCubicSpline;
}

//---------------------------------------------------------------------------
//  void EvaluateClampedCubicSplineVectorized(
//                              const Rvector &a, const Rvector &b,
//                              const Rvector &c, const Rvector &d,
//                              const Rvector &xArray, Real x,
//                              Real &y, Real &dy, Real &ddy)
//---------------------------------------------------------------------------
/**
 * Evaluate the cubic spline for a vector of states. In each region,
 * y = a_i + b_i * (x - x_i) + c_i * (x - x_i)^2 + d_i * (x - x_i)^3
 *
 * @param a      [in] Array of spline coefficients for all states and all times,
 *                    organized such that a[i][j] is coefficient for time i and
 *                    state j; constant coefficient
 * @param b      [in] Array of spline coefficients: for all states and all times,
 *                    organized such that a[i][j] is coefficient for time i and
 *                    state j; linear coefficient
 * @param c      [in] Array of spline coefficients for all states and all times,
 *                    organized such that a[i][j] is coefficient for time i and
 *                    state j; quadratic coefficient
 * @param d      [in] Array of spline coefficients for all states and all times,
 *                    organized such that a[i][j] is coefficient for time i and
 *                    state j; cubic coefficient
 * @param xArray [in] The values of the independent variable used to generate
 *                    the spline as an array
 * @param x      [in] Value of independent variable at which spline is desired
 * @param y      [out] The splined values of the dependent variable at x
 * @param dy     [out] Splined dy/dx
 * @param ddy    [out] Splined d^2y/dx^2
 */
//---------------------------------------------------------------------------
void CubicSpline::EvaluateClampedCubicSplineVectorized(
                                          const Rmatrix &a, const Rmatrix &b,
                                          const Rmatrix &c, const Rmatrix &d,
                                          const Rvector &xArray, Real x,
                                          Rvector &y, Rvector &dy, Rvector &ddy)
{
   /// @todo Add validation to make sure input matrices and vectors are sized
   /// correctly
   /// a, b, c, d should be the same size
   /// xarray size should match number of columns in a, b, c, d
   
   // calculate where we are in the spline (i.e., the value of i s.t.
   // x[i] <= x < x[i+1], assuming all x[i+1] > x[i])
   Integer s = xArray.GetSize();
   Integer numRows, numCols;
   a.GetSize(numRows, numCols);
   
   Integer idx = -1;
   if (x < xArray(0))
      idx = 0;
   else
   {
      for (Integer ii = 0; ii < s-2; ii++)
      {
         if ((x == xArray(ii))   ||
             ((x > xArray(ii)) && (x < xArray(ii+1))) )
         {
            idx = ii;
            break;
         }
         else
            continue;
      }
      if (idx == -1) // x > last element of the array
         idx = s-2;
   }

   Rvector aNeeded = a.GetRow(idx);
   Rvector bNeeded = b.GetRow(idx);
   Rvector cNeeded = c.GetRow(idx);
   Rvector dNeeded = d.GetRow(idx);
   
   Real dx = x - xArray(idx);
   
   Real dxdNeeded;
   Real threeDxdNeeded;
   
   y.SetSize(numCols);
   dy.SetSize(numCols);
   ddy.SetSize(numCols);
   
   for (Integer ii = 0; ii < numCols; ii++)
   {
      dxdNeeded      = dx * dNeeded(ii);
      threeDxdNeeded = 3.0 * dxdNeeded;
      y(ii)   = aNeeded(ii) + dx * (bNeeded(ii) + dx * (cNeeded(ii) + dxdNeeded));
      dy(ii)  = bNeeded(ii) + dx * (2.0 * cNeeded(ii) + threeDxdNeeded);
      ddy(ii) = 2.0 * (cNeeded(ii) + threeDxdNeeded);
   }
   // returning  y, dy, ddy via argument list
}

//---------------------------------------------------------------------------
//  void CalculateClampedCubicSplineCoefficients(const Rvector &x, const Rvector &y,
//                                               Real    dydx0, Real dydxf,
//                                               Rvector &a,    Rvector &b,
//                                               Rvector &c,    Rvector &d)
//---------------------------------------------------------------------------
/**
 * Calculate constants for a clamped cubic spline
 * Algorith from http://macs.citadel.edu/chenm/343.dir/09.dir/lect3_4.pdf
 *
 * @param x      [in] Array of values of independent variable
 * @param y      [in] Array of values of dependent variable
 * @param dydx0  [in] dy/dx at x[0]
 * @param dydxf  [in] dy/dx at x[n-1]
 * @param a      [out] Array of spline coefficients: constant coefficient
 * @param b      [out] Array of spline coefficients: linear coefficient
 * @param c      [out] Array of spline coefficients: quadratic coefficient
 * @param d      [out] Array of spline coefficients: cubic coefficient
 */
//---------------------------------------------------------------------------
void CubicSpline::CalculateClampedCubicSplineCoefficients(
                           const Rvector &x,   const Rvector &y,
                           Real          dydx0,Real          dydxf,
                           Rvector       &a,   Rvector       &b,
                           Rvector       &c,   Rvector       &d)
{
   #ifdef DEBUG_ARRAYS
      MessageInterface::ShowMessage("In CalculateClampedCubicSplineCoefficients\n");
   #endif
   // the step sizes
   Integer np1 = x.GetSize(); // number of knots
   Integer n   = np1 - 1;     // number of spline regions
   Rvector h(n);              // step size of each region
   Rvector oneByH(n);         // inverse of step size for each region
   Rvector dy(n);             // difference between y values and start
                              // and end of each region
   
   // populate the arrays
   for (Integer i = 0; i < n; i++)
   {
      h(i)      = x(i+1)-x(i);
      oneByH(i) = 1.0 / h(i);
      dy(i)     = y(i+1)-y(i);
   }
   
   // the right-hand side vector
   Rvector v(np1);
   
   v(0) = oneByH(0) * dy(0) - dydx0;
   for (Integer i = 1; i < n; i++)
   {
      v(i) = oneByH(i) * dy(i) - oneByH(i-1) * dy(i-1);
   }
   v(n) = dydxf - oneByH(n-1) * dy(n-1);
   v = 3.0 * v;

   #ifdef DEBUG_ARRAYS
      MessageInterface::ShowMessage("In CalculateClampedCubicSplineCoefficients, "
                                    "arrays set up and size = %d\n", n);
   #endif

   // create the tridiagonal system matrix
   // only need to create the diagonal terms; the others are just the step
   // sizes, which are already contained in h
   Rvector bThomas(np1);
   bThomas(0) = 2.0 * h(0);
   for (Integer i = 1; i < n; i++)
      bThomas(i) = 2.0 * (h(i-1) + h(i));
   bThomas[n] = 2.0 * h(n-1);
   
   // use Thomas Algorithm to solve the tridiagonal system
   Rvector cTmp = ThomasAlgorithm(h, bThomas, h, v);
   #ifdef DEBUG_ARRAYS
      MessageInterface::ShowMessage("In CalculateClampedCubicSplineCoefficients, "
                                    "Thomas called\n");
   #endif
   
   // Reset the sizes of the output arrays
   a.SetSize(n);
   c.SetSize(n);
   b.SetSize(n);
   d.SetSize(n);
   #ifdef DEBUG_ARRAYS
      MessageInterface::ShowMessage("In CalculateClampedCubicSplineCoefficients, "
                                    "sizes set\n");
   #endif
   
   // calculate the b and d coefficients
   for (Integer ii = 0; ii < n; ii++)
   {
      b(ii) = oneByH(ii) * (y(ii+1) - y(ii)) - (h(ii) / 3.0) *
             (2.0 * cTmp(ii) + cTmp(ii+1));
      d(ii) = (1.0 / 3.0) * oneByH(ii) * (cTmp(ii+1) - cTmp(ii));

      a(ii) = y(ii);       // only return 0->(n-1) for a, c
      c(ii) = cTmp(ii);    // only return 0->(n-1) for a, c
   }
   /// returning a, b, c, d via argument list
}

//---------------------------------------------------------------------------
//  Rvector ThomasAlgorithm(const Rvector &a, const Rvector &b,
//                          const Rvector &c, const Rvector &d)
//---------------------------------------------------------------------------
/**
 * Use Thomas Algorithm to solve a tridiagonal system
 * https://stackoverflow.com/questions/8733015/tridiagonal-matrix-algorithm-tdma-aka-thomas-algorithm-using-python-with-nump
 *
 * @param a      [in] Vector of coefficients below the diagonal (length n-1)
 * @param b      [in] Vector of coefficients on the diagonal (length n)
 * @param c      [in] Vector of coefficients above the diagonal (length n-1)
 * @param d      [in] Righthand side vector (length n)
 *
 * @return       Solution vector (length n)
 */
//---------------------------------------------------------------------------
Rvector CubicSpline::ThomasAlgorithm(const Rvector &a, const Rvector &b,
                                     const Rvector &c, const Rvector &d)
{
   /// @todo Add validation of a, b, c, d sizes
   Integer n = d.GetSize();
   Rvector w(n-1);
   Rvector g(n);
   Rvector p(n);
   
   w(0) = c(0) / b(0);
   g(0) = d(0) / b(0);
   
   for (Integer i = 1; i < n-1; i++) //  in range(1,n-1):
   {
      w(i) = c(i) / (b(i) - a(i-1) * w(i-1));
   }
   for (Integer i = 1; i < n; i++)
   {
      g(i) = (d(i) - a(i-1)*g(i-1)) / (b(i) - a(i-1) * w(i-1));
   }
   p(n-1) = g(n-1);
   
   for (Integer i = n-1; i > 0; i--)
   {
      p(i-1) = g(i-1) - w(i-1) * p(i);
   }
   
   return p;
}

//---------------------------------------------------------------------------
//  Real FiniteDifferenceAtEdge(const Rvector &xArray, const Rvector &yArray,
//                              const std::string &leftOrRightEdge)
//---------------------------------------------------------------------------
/**
 * Use a fourth-order, single-sided numerical difference scheme to approximate
 * a derivative. Mostly used so that we can use a clamped cubic spline.
 *
 * @param xArray          [in] Vector of coefficients below the diagonal 
 *                             (length n-1) expecting 5??
 * @param yArray          [in] Vector of coefficients on the diagonal 
 *                             (length n)   expecting 5??
 * @param leftOrRightEdge [in] Vector of coefficients above the diagonal 
 *                             (length n-1)
 * @return                derivative
 */
//---------------------------------------------------------------------------
Real CubicSpline::FiniteDifferenceAtEdge(
                           const Rvector &xArray, const Rvector &yArray,
                           const std::string &leftOrRightEdge)
{
   /// @todo Add validation of xArray and yArray sizes - must be 5?
   Real h      = xArray(1) - xArray(0);
   Rvector coefArray;
   if (leftOrRightEdge == "left") //  left edge
      coefArray.Set(5, (-25.0 / 12.0), 4.0, -3.0, (4.0 / 3.0), (-1.0 / 4.0));
   else if (leftOrRightEdge == "right")  // right edge
      coefArray.Set(5, (1.0 / 4.0), (-4.0 / 3.0), 3.0, -4.0, (25.0/ 12.0));
   else
      throw UtilityException("ERROR in FiniteDifferenceAtEdge\n");
   
   // Dot product of coefficients with yArray
   Real dydx = coefArray * yArray;
   dydx = dydx / h;
   
   return dydx;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------

CubicSpline::CubicSpline()
{
   // unimplemented
}

CubicSpline::CubicSpline(const CubicSpline &copy)
{
   // unimplemented
}

