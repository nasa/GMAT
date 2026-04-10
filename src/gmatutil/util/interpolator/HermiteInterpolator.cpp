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

#include "HermiteInterpolator.hpp"
#include "InterpolatorException.hpp"
#include "MessageInterface.hpp"


//#define DUMP_INTERPOLATOR_DATA

//------------------------------------------------------------------------------
// HermiteInterpolator()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
HermiteInterpolator::HermiteInterpolator(const std::string &name, Integer dim,
      Integer points) :
   Interpolator            (name, "HermiteInterpolator", dim),
   pointsWanted            (points),
   interpolateNewtonian    (true)
{
   bufferSize = pointsWanted+1;
}


//------------------------------------------------------------------------------
// ~HermiteInterpolator()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
HermiteInterpolator::~HermiteInterpolator()
{
   CleanupArrays();
}

//------------------------------------------------------------------------------
// HermiteInterpolator::HermiteInterpolator(const HermiteInterpolator &hi)
//------------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param hi Interpolator used to configure this one
 */
//------------------------------------------------------------------------------
HermiteInterpolator::HermiteInterpolator(const HermiteInterpolator &hi) :
   Interpolator            (hi),
   pointsWanted            (hi.pointsWanted),
   interpolateNewtonian    (hi.interpolateNewtonian)
{
}


//------------------------------------------------------------------------------
// HermiteInterpolator& operator=(const HermiteInterpolator &hi)
//------------------------------------------------------------------------------
/**
 * Assignment operator
 *
 * @param hi Interpolator used to set up this one
 *
 * @return this Interpoaltor, set to look like hi
 */
//------------------------------------------------------------------------------
HermiteInterpolator& HermiteInterpolator::operator=(const HermiteInterpolator &hi)
{
   if (this != &hi)
   {
      Interpolator::operator=(hi);

      pointsWanted         = hi.pointsWanted;
      interpolateNewtonian = hi.interpolateNewtonian;

      CleanupArrays();
   }

   return *this;
}


//------------------------------------------------------------------------------
// Interpolator* Clone() const
//------------------------------------------------------------------------------
/**
 * Creates a copy of this Interpolator
 */
//------------------------------------------------------------------------------
Interpolator* HermiteInterpolator::Clone() const
{
   return new HermiteInterpolator(*this);
}


//------------------------------------------------------------------------------
// void Clear()
//------------------------------------------------------------------------------
/**
 * cleans up the data arrays in prop for a new interpolation span
 */
//------------------------------------------------------------------------------
void HermiteInterpolator::Clear()
{
   derivatives.clear();
   qCoeffs.clear();
   tValues.clear();
   Interpolator::Clear();
}


//------------------------------------------------------------------------------
// bool AddDerivative(const Real ind, const Real *data, const Integer order)
//------------------------------------------------------------------------------
/**
 * Hermite interpolation requires derivative data
 *
 * @param ind   The value of the independent parameter associated with the
 *              derivative data.
 * @param data  The derivative data values.  Use -9.99999e99 (or smaller) to
 *              indicate that no derivative data exists for a given element.
 *              The data vector should have the same size (dimension) as the
 *              dependent data points.
 * @param order The order of the derivative.  THe current code only uses first
 *              derivative data, so the default (1) is the only order used.
 *
 * @return true if at least one derivative value is set, otherwise false.
 */
//------------------------------------------------------------------------------
bool HermiteInterpolator::AddDerivative(const Real ind, const Real *data,
      const Integer order)
{
   bool retval = false;
   Integer dvIndex = order - 1;

   if (order != 1)
      throw InterpolatorException("The Hermite interpolator is only configured "
            "through first order derivatives.");

   // Setup the data structure if needed: derivatives[element][point][deriv]
   if (derivatives.size() == 0)
   {
      for (Integer i = 0; i < dimension; ++i)
      {
         std::vector<RealArray> dvi;
         for (Integer j = 0; j < pointCount; ++j)
         {
            RealArray dv;
            dvi.push_back(dv);
         }
         derivatives.push_back(dvi);
      }
   }

   // Find the index of the point receiving derivative data
   Integer index = -1;
   for (Integer j = 0; j < pointCount; ++j)
      if (independent[j] == ind)
      {
         index = j;
         break;
      }

   for (Integer i = 0; i < dimension; ++i)
   {
      // Derivative values must be larger than -9.99999e99: that value
      // indicates no derivative available at the current order
      if (data[i] > -9.99999e99)
      {
         if (derivatives[i][index].size() > dvIndex)
         {
            derivatives[i][index][dvIndex] = data[i];
            retval = true;
         }
         else if (derivatives[i][index].size() == dvIndex)
         {
            derivatives[i][index].push_back(data[i]);
            retval = true;
         }
         else
            throw InterpolatorException("Derivative data must be added to the "
                  "Hermite interpolator order by order, starting with the "
                  "first derivative and ending with the highest order "
                  "derivative available.");
      }
   }

   return retval;
}

//---------------------------------------------------------------------------
// bool Interpolate(const Real ind, Real *results)
//---------------------------------------------------------------------------
/**
 * Interpolate the data.
 *
 * Derived classes implement this method to provide the mathematics that
 * perform the data interpolation, resulint in an array of interpolated data
 * valid at the desired value of the independent variable.
 *
 * @param ind     Value of the independent variable at which the data is
 *                interpolated.
 * @param results Array of interpolated data.
 *
 * @return true on success, false (or throw) on failure.
 */
//---------------------------------------------------------------------------
bool HermiteInterpolator::Interpolate(const Real ind, Real *results)
{
   bool retval = false;

   #ifdef DUMP_INTERPOLATOR_DATA
      MessageInterface::ShowMessage("Interpolating from %d points and %d "
            "derivatives\n", pointCount,
            (derivatives.size() == 0 ? 0 : derivatives[0][0].size()));

      for (Integer j = 0; j < dimension; ++j)
      {
         MessageInterface::ShowMessage("   Element %d:\n", j);
         for (UnsignedInt i = 0; i < pointCount; ++i)
         {
            MessageInterface::ShowMessage("     %d:  t: %lf  f(t):  %le", i,
                  independent[i], dependent[i][j]);
            if (derivatives.size() > 0)
            {
               if (derivatives[j].size() > 0)
               {
                  if (derivatives[j][i].size() > 0)
                  {
                     MessageInterface::ShowMessage("   Dv:  ");
                     for (UnsignedInt k = 0; k < derivatives[j][i].size(); ++k)
                        MessageInterface::ShowMessage("%.10le  ", derivatives[j][i][k]);
                  }
               }
            }
            MessageInterface::ShowMessage("\n");
         }
         MessageInterface::ShowMessage("\n");
      }
   #endif

   if (interpolateNewtonian)
   {
      if (BuildQCoefficients())
         retval = EvaluatePolynomial(ind, results);
   }
   else
   {
      throw InterpolatorException("Hermite Lagrange interpolation is not yet "
            "implemented.");
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool            InterpolateCartesianState(const Real ind, Real *results)
//------------------------------------------------------------------------------
/**
 *
 *
 * @param
 *
 * @return
 */
//------------------------------------------------------------------------------
bool HermiteInterpolator::InterpolateCartesianState(const Real ind, Real *results)
{
   bool retval = false;

   if (dimension != 6)
      throw InterpolatorException("HermiteInterpolator::InterpolateCartesian"
            "State requires a 6 element set of basis points; other dimensions "
            "are not supported.");

   if (interpolateNewtonian)
   {
      Real derivative[6];
      if (BuildQCoefficients())
      {
         retval = EvaluatePolynomial(ind, results);
         if (retval)
         {
            retval = EvaluatePolynomialDerivative(ind, derivative);
            for (Integer i = 0; i < 3; ++i)
               results[i+3] = derivative[i];
         }
      }
   }
   else
   {
      throw InterpolatorException("Hermite Lagrange interpolation is not yet "
            "implemented.");
   }

   return retval;
}


//------------------------------------------------------------------------------
// Methods used for Newtonian divided difference polynomials
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// bool BuildprevCol()
//------------------------------------------------------------------------------
/**
 * Hermite method used to construct divided difference coefficients
 *
 * Constructs the coefficients of the Hermite polynomial by walking through the
 * prevCol creation mechanism
 *
 * @return The top row of the divided difference prevCol -- i.e. the Hermite
 *         polynomial coefficients
 */
//------------------------------------------------------------------------------
bool HermiteInterpolator::BuildQCoefficients()
{
   bool retval = false;

   // Start from nothing
   qCoeffs.clear();
   RealArray qs, x;


   // We only need the prevCol one column at a time
   RealArray prevCol, tableau;
   tValues.clear();

   // Walk through element by element
   for (UnsignedInt i = 0; i < dimension; ++i)
   {
      // Setup the array for this element
      qCoeffs.push_back(qs);

      // Load the variable data: qCoeffs[i][0] = f(x_i)
      qCoeffs[i].push_back(dependent[0][i]);

      // Build the divided difference queue
      prevCol.clear();
      tableau.clear();
      x.clear();

      // Requirement: The same derivative order for every point for this element
      Integer dvSize = (derivatives.size() == 0 ? 0 :
            (derivatives[i].size() == 0 ? 0 : derivatives[i][0].size()));

      // Initial prep
      for (Integer m = 0; m < pointCount; ++m)
      {
         for (Integer n = 0; n < dvSize+1; ++n)
         {
            // If we allow for absent derivative data, adjust things here
            if ((n > 0) && (derivatives[i][m].size() == 0))
               throw InterpolatorException("The derivative data provided is "
                     "inconsistent: some elements of a component of the "
                     "interpolation vector have derivatives while other "
                     "elements do not.");
            x.push_back(independent[m]);
            prevCol.push_back(dependent[m][i]);
         }
      }

      Integer order = pointCount * (dvSize + 1) - 1;

      #ifdef DUMP_INTERPOLATOR_DATA
         MessageInterface::ShowMessage("Element %d initial column Data:\n", i);

         MessageInterface::ShowMessage("   Initial data: Ind, Dep\n");
         for (UnsignedInt j = 0; j < x.size(); ++j)
         {
            MessageInterface::ShowMessage("      %d:  %.10lf   %.10le\n", j,
                  x[j], prevCol[j]);
         }
      #endif

      // Initialize the tableau with derivative data, if available
      Integer point = 0, tindex = 1;

      for (UnsignedInt t = 0; t < order; ++t)
      {
         for (UnsignedInt j = 0; j < prevCol.size()-1; ++j)
         {
            Real deriv;
            if (x[j+tindex] != x[j])
            {
               deriv = (prevCol[j+1] - prevCol[j]) / (x[j+tindex] - x[j]);
               ++point;
            }
            else
               deriv = derivatives[i][point][0];
            tableau.push_back(deriv);
         }

         #ifdef DUMP_INTERPOLATOR_DATA
            MessageInterface::ShowMessage("   Tableau:");
            for (UnsignedInt j = 0; j < tableau.size(); ++j)
               MessageInterface::ShowMessage("   %.10le", tableau[j]);
            MessageInterface::ShowMessage("\n");
         #endif

         qCoeffs[i].push_back(tableau[0]);

         prevCol = tableau;
         tableau.clear();
         ++tindex;
      }

      tValues.push_back(x);
      retval = true;
   }

   #ifdef DUMP_INTERPOLATOR_DATA
      MessageInterface::ShowMessage("Q matrix:\n");
      for (UnsignedInt i = 0; i < qCoeffs.size(); ++i)
      {
         MessageInterface::ShowMessage("   %d:  ", i);
         for (UnsignedInt j = 0; j < qCoeffs[i].size(); ++j)
            MessageInterface::ShowMessage("   %.10le", qCoeffs[i][j]);
         MessageInterface::ShowMessage("\n");
      }
      MessageInterface::ShowMessage("\n");
   #endif

   return retval;
}


//------------------------------------------------------------------------------
// bool EvaluatePolynomial(const Real ind, Real *results)
//------------------------------------------------------------------------------
/**
 * Method that builds the interpolated values
 *
 * @param ind The value of the independent parameter at which to interpolate
 * @param results The interpolated data
 *
 * @return true on success, false on failure
 */
//------------------------------------------------------------------------------
bool HermiteInterpolator::EvaluatePolynomial(const Real ind, Real *results)
{
   bool retval = true;

   // Walk through element by element
   for (UnsignedInt i = 0; i < dimension; ++i)
   {
      Real tProduct = 1.0;
      results[i] = 0.0;
      for (UnsignedInt j = 0; j < qCoeffs[i].size(); ++j)
      {
         if (j > 0)
         {
            tProduct *= (ind - tValues[i][j-1]);
            #ifdef DUMP_INTERPOLATOR_DATA
               MessageInterface::ShowMessage(" %d: %.12lf - %.12lf = %.12lf "
                     "==> P = %.12lf ==> ", ind, tValues[i][j],
                     ind - tValues[i][j], tProduct);
            #endif
         }
         results[i] += qCoeffs[i][j] * tProduct;

         #ifdef DUMP_INTERPOLATOR_DATA
            MessageInterface::ShowMessage(" %.12lf\n", results[i]);
         #endif
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// bool EvaluatePolynomialDerivative(const Real ind, Real *results)
//------------------------------------------------------------------------------
/**
 * Generates derivative data for the interpolating polynomial
 *
 * @param ind The independent parameter value at which the derivative is needed
 * @param results The container for the derivative data
 *
 * @return true on success, false on failure (currently always true)
 */
//------------------------------------------------------------------------------
bool HermiteInterpolator::EvaluatePolynomialDerivative(const Real ind, Real *results)
{
   bool retval = true;

   // Walk through element by element
   for (UnsignedInt i = 0; i < dimension; ++i)
   {
      Real tProduct;
      results[i] = 0.0;
      for (UnsignedInt j = 1; j < qCoeffs[i].size(); ++j)
      {
         tProduct = EvaluateDerivativeIndependentTerm(ind, j, i);
         results[i] += qCoeffs[i][j] * tProduct;
      }
   }

   return retval;
}


//------------------------------------------------------------------------------
// Real EvaluateDerivativeIndependentTerm(Integer ind, Integer order)
//------------------------------------------------------------------------------
/**
 * Evaluate independent variable part of the derivative polynomial
 *
 * @param ind The independent parameter value at which the derivative is needed
 * @param order The order of the Q coefficient that needs the computed value
 * @param forElement The interpolent element that is currently being evaluated
 *
 * @return The coefficient of teh polynomial term used in teh evaluation
 */
//------------------------------------------------------------------------------
Real HermiteInterpolator::EvaluateDerivativeIndependentTerm(const Real ind,
      Integer order, Integer forElement)
{
   Real tt = 0.0, prod;

   if (order > 1)
   {
      for (Integer i = 0; i < order; ++i)
      {
         prod = 1.0;
         for (Integer j = 0; j < order; ++j)
         {
            if (j != i)
               prod *= (ind - tValues[forElement][j]);
         }
         tt += prod;
      }
   }
   else
      tt = 1.0;

   return tt;
}

////------------------------------------------------------------------------------
//// Methods used for Lagrange polynomials
////------------------------------------------------------------------------------
//
////------------------------------------------------------------------------------
//// void EvaluateDerivative(const Integer atPoint, const Integer ofOrder,
////          Real *deriv, const Integer forCount = 3)
////------------------------------------------------------------------------------
///**
// *
// *
// * @param
// *
// * @return
// */
////------------------------------------------------------------------------------
//void HermiteInterpolator::EvaluateDerivative(const Integer atPoint,
//      const Integer ofOrder, Real *deriv, const Integer forCount)
//{
//
//}
//
//
////------------------------------------------------------------------------------
//// Real Alpha()
////------------------------------------------------------------------------------
///**
// *
// *
// * @param
// *
// * @return
// */
////------------------------------------------------------------------------------
//Real HermiteInterpolator::Alpha()
//{
//
//}
//
//
////------------------------------------------------------------------------------
//// Real Beta()
////------------------------------------------------------------------------------
///**
// *
// *
// * @param
// *
// * @return
// */
////------------------------------------------------------------------------------
//Real HermiteInterpolator::Beta()
//{
//
//}
