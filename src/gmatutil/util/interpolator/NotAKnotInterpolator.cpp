//$Id$
//------------------------------------------------------------------------------
//                            NotAKnotInterpolator
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
 * Defines interpolation class using the cubic spline algorithm with "not a 
 * knot" conditions at points 2 and 4, as described in the GMAT Math Spec.
 */
//------------------------------------------------------------------------------


#include "NotAKnotInterpolator.hpp"
#include "MessageInterface.hpp"

//#define DUMP_SPLINE_POINTS

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  NotAKnotInterpolator(const std::string &name, Integer dim)
//------------------------------------------------------------------------------
/**
 * Constructs not a knot interpolator (default constructor).
 * 
 * @param <name> Name for this interpolator.
 * @param <dim>  The dimension of the vector that is interpolated.
 */
//------------------------------------------------------------------------------
NotAKnotInterpolator::NotAKnotInterpolator(const std::string &name,
                                           Integer dim) :
   Interpolator        (name, "NotAKnotInterpolator", dim),
   lastX               (-9.9999e75)
{
   bufferSize = 5;
   requiredPoints = 5;
   Integer i;
   for (i = 0; i < bufferSize; ++i)
   {
      x[i]  = -9.9999e75;
      y[i]  = NULL;
   }

   for (Integer j = 0; j < 4; ++j)
   {
      a[j] = b[j] = c[j] = d[j] = delta[j] = NULL;
   }
   
   B[0] = B[1] = B[2] = NULL;
   s[0] = s[1] = s[2] = s[3] = s[4] = NULL;
}


//------------------------------------------------------------------------------
//  ~NotAKnotInterpolator()
//------------------------------------------------------------------------------
/**
 * Destroys not a knot interpolator (destructor).
 */
//------------------------------------------------------------------------------
NotAKnotInterpolator::~NotAKnotInterpolator()
{   
   CleanupArrays();
}


//------------------------------------------------------------------------------
//  NotAKnotInterpolator(const NotAKnotInterpolator &csi)
//------------------------------------------------------------------------------
/**
 * Constructs not a knot interpolator, based on another (copy constructor).
 * 
 * @param csi The original that is being copied.
 */
//------------------------------------------------------------------------------
NotAKnotInterpolator::NotAKnotInterpolator(const NotAKnotInterpolator &csi) :
   Interpolator       (csi),
   lastX              (csi.lastX)
{
   B[0]  = NULL;
   B[1]  = NULL;
   B[2]  = NULL;
   s[0]  = NULL;
   s[1]  = NULL;
   s[2]  = NULL;
   s[3]  = NULL;
   s[4]  = NULL;
   
   CopyArrays(csi);
   
   Integer i, j;
   for (i = 0; i < bufferSize; ++i)
   {
      x[i]  = -9.9999e75;
      y[i]  = NULL;
   }
   
   for (i = 0; i < 4; i++)
      h[i] = csi.h[i];
      
   for (i = 0; i < 3; i++)
   {
      for (j = 0; j < 3; j++)
         A[i][j] = csi.A[i][j];
   }
}


//------------------------------------------------------------------------------
//  NotAKnotInterpolator& operator=(const NotAKnotInterpolator &csi)
//------------------------------------------------------------------------------
/**
 * Sets this not a knot interpolator to match another (assignment operator).
 * 
 * @param csi The original that is being copied.
 * 
 * @return A reference to the copy (aka *this).
 */
//------------------------------------------------------------------------------
NotAKnotInterpolator& NotAKnotInterpolator::operator=
                                            (const NotAKnotInterpolator &csi)
{
   if (&csi == this)
      return *this;
   
   CleanupArrays();
   
   lastX = csi.lastX;
   
   CopyArrays(csi);
   
   Integer i, j;
   
   for (i = 0; i < 4; i++)
      h[i] = csi.h[i];
      
   for (i = 0; i < 3; i++)
   {
      for (j = 0; j < 3; j++)
         A[i][j] = csi.A[i][j];
   }
   
   return *this;
}


//------------------------------------------------------------------------------
//  bool Interpolate(const Real ind, Real *results)
//------------------------------------------------------------------------------
/**
 * Perform the interpolation.
 * 
 * This method is the core interface for the not a knot interpolation.  It 
 * calls protected methods to build the splines and to estimate the state at the
 * input value of the independent parameter (ind).
 * 
 * @param ind       The value of the independent parameter.
 * @param results   Data structure for the estimates.
 * 
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool NotAKnotInterpolator::Interpolate(const Real ind, Real *results)
{
   if (pointCount < requiredPoints)
      throw InterpolatorException("ERROR - NotAKnotInterpolator: " +
         GmatStringUtil::ToString(requiredPoints, 1) + " points "
         "are required for interpolation, but only " +
         GmatStringUtil::ToString(pointCount, 1) + " were provided.\n");
        
   bool retval = BuildSplines();
   if (retval)
      retval = Estimate(ind, results);
      
   #ifdef DUMP_SPLINE_POINTS
      MessageInterface::ShowMessage("Cubic spline uses these points:\n");
      for (Integer i = 0; i < 5; ++i)
      {
         MessageInterface::ShowMessage("   %.12lf:", x[i]);
         for (Integer j = 0; j < dimension; ++j)
            MessageInterface::ShowMessage("   %.12lf", y[i][j]);
         MessageInterface::ShowMessage("\n");
      }
         
      MessageInterface::ShowMessage("Cubic spline (not a knot) data points:\n");
      
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
      
      MessageInterface::ShowMessage("Spline Estimate:\n");
      for (Integer i = 0; i < dimension; ++i)
         MessageInterface::ShowMessage("   %02d:   %.12lf\n", i, results[i]);
      
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
void NotAKnotInterpolator::AllocateArrays()
{
   Interpolator::AllocateArrays();

   Integer i;
   for (i = 0; i < bufferSize; ++i)
   {
      y[i]  = new Real[dimension];
   }
   
   for (Integer j = 0; j < 4; ++j)
   {
      a[j] = new Real[dimension];
      b[j] = new Real[dimension];
      c[j] = new Real[dimension];
      d[j] = new Real[dimension];
      delta[j] = new Real[dimension];
   }
   
   B[0] = new Real[dimension];
   B[1] = new Real[dimension];
   B[2] = new Real[dimension];
   s[0] = new Real[dimension];
   s[1] = new Real[dimension];
   s[2] = new Real[dimension];
   s[3] = new Real[dimension];
   s[4] = new Real[dimension];   
    
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
void NotAKnotInterpolator::CleanupArrays()
{
   if (y[0])
   {
      for (Integer i = 0; i < bufferSize; ++i)
      {
         delete [] y[i];
         y[i] = NULL;
      }
      
      for (Integer j = 0; j < 4; ++j)
      {
         delete [] a[j];
         a[j] = NULL;
         delete [] b[j];
         b[j] = NULL;
         delete [] c[j];
         c[j] = NULL;
         delete [] d[j];
         d[j] = NULL;
         delete [] delta[j];
         delta[j] = NULL;
      }
      
      delete [] B[0];
      delete [] B[1];
      delete [] B[2];
      delete [] s[0];
      delete [] s[1];
      delete [] s[2];
      delete [] s[3];
      delete [] s[4];

      B[0] = NULL;
      B[1] = NULL;
      B[2] = NULL;
      s[0] = NULL;
      s[1] = NULL;
      s[2] = NULL;
      s[3] = NULL;
      s[4] = NULL;
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
void NotAKnotInterpolator::CopyArrays(const NotAKnotInterpolator &i)
{
   Interpolator::CopyArrays(i);
   Integer j;
   for (j = 0; j < bufferSize; ++j)
   {
      x[j] = i.x[j];
      memcpy( y[j],  i.y[j], dimension*sizeof(Real));
   }
   for (Integer j = 0; j < 4; ++j)
   {
      memcpy(a[j],  i.a[j], dimension*sizeof(Real));
      memcpy(b[j],  i.b[j], dimension*sizeof(Real));
      memcpy(c[j],  i.c[j], dimension*sizeof(Real));
      memcpy(d[j],  i.d[j], dimension*sizeof(Real));
      memcpy(delta[j],  i.delta[j], dimension*sizeof(Real));
   }
}


//------------------------------------------------------------------------------
//  bool BuildSplines()
//------------------------------------------------------------------------------
/**
 * Uses the data in the ring buffer to build the not a knot splines.
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
bool NotAKnotInterpolator::BuildSplines()
{
   // Set x and y from the ring buffer
   LoadArrays();
   
   for (Integer i = 0; i < bufferSize-1; ++i)
   {
      h[i] = x[i+1] - x[i];
      if (h[i] == 0.0)
         return false;
            
      for (Integer j = 0; j < dimension; ++j)
      {
         delta[i][j] = (y[i+1][j] - y[i][j]) / h[i];
      }
   }
   
   A[0][0] = 2.0 * h[1] + h[0];
   A[0][1] = 2.0 * h[0] + h[1];
   A[0][2] = 0.0;
   A[1][0] = 0.0;
   A[1][1] = h[2] + 2.0 * h[3];
   A[1][2] = 2.0 * h[2] + h[3];

   Real denom = h[0] + h[1];
   if (denom == 0.0)
      return false;
   Real denom2 = h[2] + h[3];
   if (denom2 == 0.0)
      return false;

   A[2][0] = h[1]*h[1] / denom;
   A[2][1] = h[0]*h[1] / denom + 2.0*(h[1] + h[2]) + h[2]*h[3] / denom2;
   A[2][2] = h[2]*h[2] / denom2;
   
   Real detA = A[0][0] * (A[1][1] * A[2][2] - A[1][2] * A[2][1]) + 
               A[0][1] * (A[1][2] * A[2][0] - A[1][0] * A[2][2]) + 
               A[0][2] * (A[1][0] * A[2][1] - A[1][1] * A[2][0]);
               
   if (detA == 0.0)
      return false;
   
   if (A[2][2] == 0.0)
      return false;

   for (Integer i = 0; i < dimension; ++i)
   {
      B[0][i] = 6.0 * (delta[1][i] - delta[0][i]);
      B[1][i] = 6.0 * (delta[3][i] - delta[2][i]);
      B[2][i] = 6.0 * (delta[2][i] - delta[1][i]);
      
      s[0][i] = (B[0][i] * (A[1][1] * A[2][2] - A[1][2] * A[2][1]) +
                 A[0][1] * (A[1][2] * B[2][i] - B[1][i] * A[2][2]) + 
                 A[0][2] * (B[1][i] * A[2][1] - A[1][1] * B[2][i])) / detA;
      
      s[2][i] = (A[0][0] * (B[1][i] * A[2][2] - A[1][2] * B[2][i]) +
                 B[0][i] * (A[1][2] * A[2][0] - A[1][0] * A[2][2]) + 
                 A[0][2] * (A[1][0] * B[2][i] - B[1][i] * A[2][0])) / detA;
      
      s[4][i] = (B[2][i] - A[2][0]*s[0][i] - A[2][1]*s[2][i]) / A[2][2];
      
      s[1][i] = (h[1] * s[0][i] + h[0] * s[2][i]) / denom;
      
      s[3][i] = (h[3] * s[2][i] + h[2] * s[4][i]) / denom2;
      
      for (Integer j = 0; j < 4; ++j)
      {
         a[j][i] = (s[j+1][i] - s[j][i]) / (6.0 * h[j]);
         b[j][i] = s[j][i] / 2.0;
         c[j][i] = (y[j+1][i] - y[j][i]) / h[j] - 
                   (2.0 * h[j] * s[j][i] + h[j] * s[j+1][i]) / 6.0;
         d[j][i] = y[j][i];
      }
   }
   
   return true;
}


//------------------------------------------------------------------------------
//  void LoadArrays()
//------------------------------------------------------------------------------
/**
 * Use the ring buffer to load the arrays used to build the splines.
 */
//------------------------------------------------------------------------------
void NotAKnotInterpolator::LoadArrays()
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
bool NotAKnotInterpolator::Estimate(const Real ind, Real *results)
{
   Integer i, kl = -1, kh;
   Real h;

   // First find the spline we want
   for (i = 0; i < 4; ++i)
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
    
   Real dx = ind - x[kl];

   // Finally, evaluate the spline!
   for (i = 0; i < dimension; ++i)
   {
      results[i] = a[kl][i] * (dx*dx*dx) + b[kl][i] * (dx*dx) +
                   c[kl][i] * dx + d[kl][i];
   }
    
   return true;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the NotAKnotInterpolator.
 *
 * @return clone of the NotAKnotInterpolator.
 */
//------------------------------------------------------------------------------
Interpolator* NotAKnotInterpolator::Clone() const
{
   return (new NotAKnotInterpolator(*this));
}

