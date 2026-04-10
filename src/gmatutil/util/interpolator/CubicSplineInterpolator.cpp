//$Id$
//------------------------------------------------------------------------------
//                            CubicSplineInterpolator
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
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
// Author: Darrel J. Conway, Thinking Systems, Inc.
// Created: 2003/12/02
//
/**
 * Implements interpolation using the cubic spline algorithm described in 
 * Numerical Recipes in C, 2nd Ed., pp 113 ff
 */
//------------------------------------------------------------------------------


#include "CubicSplineInterpolator.hpp"
#include "MessageInterface.hpp"

//#define DUMP_SPLINE_POINTS

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  CubicSplineInterpolator(const std::string &name, Integer dim)
//------------------------------------------------------------------------------
/**
 * Constructs cubic spline interpolator (default constructor).
 * 
 * @param <name> Name for this interpolator.
 * @param <dim>  The dimension of the vector that is interpolated.
 */
//------------------------------------------------------------------------------
CubicSplineInterpolator::CubicSplineInterpolator(const std::string &name,
                                                 Integer dim) :
   Interpolator        (name, "CubicSplineInterpolator", dim),
   lastX               (-9.9999e75)
{
   bufferSize = 5;
   requiredPoints = 5;
   Integer i;
   for (i = 0; i < bufferSize; ++i)
   {
      x[i]  = -9.9999e75;
      y[i]  = NULL;
      y2[i] = NULL;
   }
}


//------------------------------------------------------------------------------
//  ~CubicSplineInterpolator()
//------------------------------------------------------------------------------
/**
 * Destroys cubic spline interpolator (destructor).
 */
//------------------------------------------------------------------------------
CubicSplineInterpolator::~CubicSplineInterpolator()
{
   CleanupArrays();
}


//------------------------------------------------------------------------------
//  CubicSplineInterpolator(const CubicSplineInterpolator &csi)
//------------------------------------------------------------------------------
/**
 * Constructs cubic spline interpolator, based on another (copy constructor).
 * 
 * @param csi The original that is being copied.
 */
//------------------------------------------------------------------------------
CubicSplineInterpolator::CubicSplineInterpolator
                                          (const CubicSplineInterpolator &csi) :
   Interpolator       (csi),
   lastX              (csi.lastX)
{
   Integer i;
   for (i = 0; i < bufferSize; ++i)
   {
      x[i]  = -9.9999e75;
      y[i]  = NULL;
      y2[i] = NULL;
   }
}


//------------------------------------------------------------------------------
//  CubicSplineInterpolator& operator=(const CubicSplineInterpolator &csi)
//------------------------------------------------------------------------------
/**
 * Sets this cubic spline interpolator to match another (assignment operator).
 * 
 * @param csi The original that is being copied.
 * 
 * @return A reference to the copy (aka *this).
 */
//------------------------------------------------------------------------------
CubicSplineInterpolator& CubicSplineInterpolator::operator=
                                            (const CubicSplineInterpolator &csi)
{
   if (&csi == this)
      return *this;

   CleanupArrays();
   
   CopyArrays(csi);
   lastX = csi.lastX;
   return *this;
}


//------------------------------------------------------------------------------
//  bool Interpolate(const Real ind, Real *results)
//------------------------------------------------------------------------------
/**
 * Perform the interpolation.
 * 
 * This method is the core interface for the cubic spline interpolation.  It 
 * calls protected methods to build the splines and to estimate the state at the
 * input value of the independent parameter (ind).
 * 
 * @param ind       The value of the independent parameter.
 * @param results   Data structure for the estimates.
 * 
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool CubicSplineInterpolator::Interpolate(const Real ind, Real *results)
{
   if (pointCount < requiredPoints)
      throw InterpolatorException("ERROR - CublicSplineInterpolator: " +
         GmatStringUtil::ToString(requiredPoints, 1) + " points "
         "are required for interpolation, but only " +
         GmatStringUtil::ToString(pointCount, 1) + " were provided.\n");
        
   bool retval = BuildSplines();
   if (retval)
      retval = Estimate(ind, results);
      
   #ifdef DUMP_SPLINE_POINTS
      MessageInterface::ShowMessage("Cubic spline data points:\n");
      
      Real increment = (x[bufferSize - 1] - x[0]) / 100.0, xval;
      Real *dumpData = new Real[dimension];
      for (Integer i = 0; i <= 100; ++i)
      {
         xval = x[0] + i * increment;
         Estimate(xval, dumpData);
         MessageInterface::ShowMessage("   %.12lf", xval);
         for (Integer j = 0; j < dimension; ++j)
            MessageInterface::ShowMessage("   %.12lf", dumpData[j]);
         MessageInterface::ShowMessage("\n");
      } 
      
      delete [] dumpData;   
   #endif

   return retval;
}


//---------------------------------
//  protected methods
//---------------------------------


//------------------------------------------------------------------------------
//  void AllocateArrays()
//------------------------------------------------------------------------------
/**
 * Allocates spline buffers and calls the base method to build the ring buffer.  
 */
//------------------------------------------------------------------------------
void CubicSplineInterpolator::AllocateArrays()
{
   Interpolator::AllocateArrays();

   Integer i;
   for (i = 0; i < bufferSize; ++i)
   {
      y[i]  = new Real[dimension];
      y2[i] = new Real[dimension];
   }
    
   latestPoint = -1;
}


//------------------------------------------------------------------------------
//  void CleanupArrays()
//------------------------------------------------------------------------------
/**
 * Frees the memory used by the spline buffer and calls the base method to 
 * manage the ring buffer.
 */
//------------------------------------------------------------------------------
void CubicSplineInterpolator::CleanupArrays()
{
   Integer i = 0;
   if (y2[i]) {
      for (i = 0; i < bufferSize; ++i)
      {
         delete [] y[i];
         y[i] = NULL;
         delete [] y2[i];
         y2[i] = NULL;
      }
   }
    
   Interpolator::CleanupArrays();
}


//------------------------------------------------------------------------------
//  void CopyArrays(const Interpolator &i)
//------------------------------------------------------------------------------
/**
 * Copies the ring buffer from one Interpolator to this one.  
 * 
 * @param i The Interpolator that supplies the data copied to this one.
 */
//------------------------------------------------------------------------------
void CubicSplineInterpolator::CopyArrays(const CubicSplineInterpolator &i)
{
   Interpolator::CopyArrays(i);
   Integer j;
   for (j = 0; j < bufferSize; ++j)
   {
      x[j] = i.x[j];
      memcpy( y[j],  i.y[j], dimension*sizeof(Real));
      memcpy(y2[j], i.y2[j], dimension*sizeof(Real));
   }
}


//------------------------------------------------------------------------------
//  bool BuildSplines()
//------------------------------------------------------------------------------
/**
 * Uses the data in the ring buffer to build the cubic splines.
 * 
 * This method uses the procedures in Numerical Recipes in C to build the 
 * coefficients used to build the splines.  Basically, the spline derivation can
 * be written as a tri-diagonal matrix equation that can be inverted to find the 
 * coefficients for the splines, using boundary value data at the knots.  The
 * resulting equations contain 16 unknown coefficients, set by 14 constraints
 * and a choice of 2 additional conditions that are not forces by the
 * constraints at the knots.
 * 
 * The constraints on the spline equations are as follows:
 * 
 *   1. Each spline must match the dependent value at its starting and ending 
 *      knots.  (5 constraints)
 * 
 *   2. The first derivatives must be continuous at the internal knots.  
 *      (3 constraints)
 * 
 *   3. The first derivatives must be smooth at the internal knots.
 *      (3 constraints)
 * 
 *   4. The second derivatives must be continuous at the internal knots.
 *      (3 constraints) 
 * 
 * This implementation builds "natural" splines -- the 2 remaining free 
 * parameters in the derivation are determined by setting the second derivatives 
 * at the endpoints to 0.0.
 * 
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool CubicSplineInterpolator::BuildSplines()
{
   Integer i, j, k;
   Real    p, sig, u[4];
    
   // Set x and y from the ring buffer
   LoadArrays();

   // Only update the splines if the data has changed
   if (x[4] == lastX)
      return true;
   lastX = x[4];
    
   // Check monotonicity -- the spline algorithm requires it.  I could throw an
   // exception here, but the calling code might prefer to drop back to an
   // alternative interpolator (e.g. a linear interpolator) or to generate more
   // densely packed data for the buffer if the cubic spline fails at this
   // stage.
   for (i = 1; i < 5; ++i)
   {
      if (dataIncreases)
      {
         if (x[i] < x[i-1]) {
            return false;
         }
      }
      else
      {
         if (x[i] > x[i-1])
         {
            return false;
         }
      }
   }
    
   // Loop through the dependent variables
   for (j = 0; j < dimension; ++j)
   {
      y2[0][j] = u[0] = 0.0;       // Natural spline constraints
        
      // Decomposition loop of the tridiagonal algorithm
      for (i = 1; i < 4; ++i)
      {
         sig = (x[i] - x[i-1]) / (x[i+1] - x[i-1]);
         p = sig * y2[i-1][j] + 2.0;
         y2[i][j] = (sig - 1.0) / p;
         u[i] = (y[i+1][j] - y[i][j]) / (x[i+1] - x[i]) -
                (y[i][j] - y[i-1][j]) / (x[i] - x[i-1]);
         u[i] = (6.0*u[i] / (x[i+1] - x[i-1]) - sig*u[i-1]) / p;
      }

      // This implementation uses "natural" spline constraints, so the second
      // derivatives vanish at the endpoint;
      y2[4][j] = 0.0;
        
      // Backsubstitution loop of the tridiagonal algorithm
      for (k = 3; k >= 0; --k)
      {
         y2[k][j] = y2[k][j] * y2[k+1][j] + u[k];
      }
   }
    
   return true;
}


//------------------------------------------------------------------------------
//  void LoadArrays(void)
//------------------------------------------------------------------------------
/**
 * Use the ring buffer to load the arrays used to build the splines.
 */
//------------------------------------------------------------------------------
void CubicSplineInterpolator::LoadArrays(void)
{
   Integer i, j, start = 0;
   Real sign = (dataIncreases ? 1.0 : -1.0);
   Real temp = sign * independent[0];

   for (i = 1; i < bufferSize; ++i)
   {
      if (sign*independent[i] < temp)
      {
         start = i;
         temp = sign*independent[i];
      }
   }
    
   for (i = 0; i < bufferSize; ++i, ++start)
   {
      if (start == bufferSize)
         start = 0;
      x[i] = independent[start];
      for (j = 0; j < dimension; ++j)
         y[i][j] = dependent[start][j];
   }
}


//------------------------------------------------------------------------------
//  bool Estimate(const Real ind, Real *results)
//------------------------------------------------------------------------------
/**
 * Perform the estimation using the information from the splines.  
 * 
 * This method is invoked after calling BuildSplines(), using the input 
 * parameters from the Interpolate() method.  It matches the "splint" routine
 * found in Numerical Recipes in C, section 3.3.
 * 
 * @param ind       The value of the independent parameter.
 * @param results   Data structure for the estimates.
 * 
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool CubicSplineInterpolator::Estimate(const Real ind, Real *results)
{
   Integer i, kl = -1, kh;
   Real h, a, b;

   // First find the spline we want -- for GMAT, this is more likely to be the
   // last one, so we'll start at the end and work backwards
   for (i = 3; i >= 0; --i)
   {
      if (dataIncreases)
      {
         if ((x[i] <= ind) && (x[i+1] >= ind))
         {
            kl = i;
            break;
         }
      }
      else
      {
         if ((x[i] >= ind) && (x[i+1] <= ind))
         {
            kl = i;
            break;
         }
      }
   }
   if (kl == -1)       // Calls for extrapolation rather than interpolation
   {
      if (allowExtrapolation)
         kl = 0;
      else 
         return false;
   }
        
   kh = kl + 1;
   h = x[kh] - x[kl];
   if (h == 0)
      return false;       // Coincident points are not allowed
    
   a = (x[kh] - ind) / h;
   b = (ind - x[kl]) / h;
    
   // Finally, evaluate the spline!
   for (i = 0; i < dimension; ++i)
   {
      results[i] = a * y[kl][i] + b * y[kh][i] +
                   ((a*a*a - a) * y2[kl][i] + (b*b*b - b) * y2[kh][i]) *
                   (h*h)/6.0;
   }
    
   return true;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the CubicSplineInterpolator.
 *
 * @return clone of the CubicSplineInterpolator.
 */
//------------------------------------------------------------------------------
Interpolator* CubicSplineInterpolator::Clone() const
{
   return (new CubicSplineInterpolator(*this));
}

