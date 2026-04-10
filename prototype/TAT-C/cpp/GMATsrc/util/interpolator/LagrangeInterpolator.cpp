//$Id$
//------------------------------------------------------------------------------
//                            LagrangeInterpolator
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
// number #####
//
// Author: Linda Jun (NASA/GSFC)
// Created: 2009/09/28
//
/**
 * Implements LagrangeInterpolator class as specified in the GMAT Math Spec.
 */
//------------------------------------------------------------------------------

#include "LagrangeInterpolator.hpp"
#include "InterpolatorException.hpp"
#include "RealUtilities.hpp"         // for GmatMathUtil::Abs()
#include "MessageInterface.hpp"

//#define DEBUG_LAGRANGE_FEASIBLE
//#define DEBUG_LAGRANGE_BUILD
//#define DEBUG_LAGRANGE_ADD
//#define DEBUG_LAGRANGE_INTERPOLATE
//#define DUMP_DATA_POINT_20

//---------------------------------
//  public methods
//---------------------------------

//------------------------------------------------------------------------------
//  LagrangeInterpolator(const std::string &name, Integer dim, Integer ord)
//------------------------------------------------------------------------------
/**
 * Constructs lagrange interpolator (default constructor).
 * 
 * @param <name>  Name for this interpolator ("")
 * @param <dim>   Dimension of data that gets interpolated (1).
 * @param <order> The order of interpolation (7)
 */
//------------------------------------------------------------------------------
LagrangeInterpolator::LagrangeInterpolator(const std::string &name, Integer dim,
                                           Integer ord) :
   Interpolator  (name, "LagrangeInterpolator", dim),
   order         (ord),
   actualSize    (0),
   beginIndex    (0),
   endIndex      (0),
   dataIndex     (0),
   startPoint    (0),
   lastX         (-9.9999e75),
   x             (NULL),
   y             (NULL)
{
   // Made bufferSize 10 times bigger than order, so that we can collect more
   // data to place requested ind parameter in the near to the center of the
   // interpolation range.
   requiredPoints = order + 1;
   bufferSize = requiredPoints * 10;
   if (bufferSize > MAX_BUFFER_SIZE)
      bufferSize = MAX_BUFFER_SIZE;
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("LagrangeInterpolator() order=%d, requiredPoints=%d, bufferSize=%d\n",
       order, requiredPoints, bufferSize);
   #endif

   AllocateArrays();
}


//------------------------------------------------------------------------------
//  ~LagrangeInterpolator()
//------------------------------------------------------------------------------
/**
 * Destroys lagrange interpolator (destructor).
 */
//------------------------------------------------------------------------------
LagrangeInterpolator::~LagrangeInterpolator()
{
   CleanupArrays();
}


//------------------------------------------------------------------------------
//  LagrangeInterpolator(const LagrangeInterpolator &li)
//------------------------------------------------------------------------------
/**
 * Constructs lagrange interpolator, based on another (copy constructor).
 * 
 * @param li The original that is being copied.
 */
//------------------------------------------------------------------------------
LagrangeInterpolator::LagrangeInterpolator(const LagrangeInterpolator &li) :
   Interpolator   (li),
   order          (li.order),
   actualSize     (li.actualSize),
   beginIndex     (li.beginIndex),
   endIndex       (li.endIndex),
   dataIndex      (li.dataIndex),
   startPoint     (li.startPoint),
   lastX          (li.lastX),
   x              (NULL),
   y              (NULL)
{
   bufferSize = li.bufferSize;
   AllocateArrays();
}


//------------------------------------------------------------------------------
//  LagrangeInterpolator& operator=(const LagrangeInterpolator &li)
//------------------------------------------------------------------------------
/**
 * Sets this lagrange interpolator to match another (assignment operator).
 * 
 * @param li The original that is being copied.
 * 
 * @return A reference to the copy (aka *this).
 */
//------------------------------------------------------------------------------
LagrangeInterpolator&
LagrangeInterpolator::operator=(const LagrangeInterpolator &li)
{
   if (&li == this)
      return *this;
   
   CleanupArrays();
   CopyArrays(li);
   
   order      = li.order;
   actualSize = li.actualSize;
   beginIndex = li.beginIndex;
   endIndex   = li.endIndex;
   dataIndex  = li.dataIndex;
   startPoint = li.startPoint;
   lastX      = li.lastX;
   
   return *this;
}


//------------------------------------------------------------------------------
//  Integer IsInterpolationFeasible(Real ind)
//------------------------------------------------------------------------------
/**
 * Checks if interpolation is feasible.
 * (Should I also check if ind is in the center of the interpolation range?)
 *
 * @param ind The value of the independent parameter.
 * @return  1 if feasible
 *         -1 if there is not enough data to interpolate
 *         -2 if requested data is before the first data
 *         -3 if requested data is after the last data
 */
//------------------------------------------------------------------------------
Integer LagrangeInterpolator::IsInterpolationFeasible(Real ind)
{
   #ifdef DEBUG_LAGRANGE_FEASIBLE
   MessageInterface::ShowMessage
      ("Lagrange::IsInterpolationFeasible() ind=%f, pointCount = %d, "
       "requiredPoints = %d\n", ind, pointCount, requiredPoints);
   #endif
   
   // If not enough data points, throw an exception
   if (pointCount < requiredPoints)
   {
      // Just return error code so that caller can handle it (LOJ: 2012.12.12)
      #if 0
      InterpolatorException ie;
      ie.SetDetails
         ("There is not enough data to interpolate %f, number of "
          "required points is %d, received %d points", ind, requiredPoints,
          pointCount);
      throw ie;
      #else
      #ifdef DEBUG_LAGRANGE_FEASIBLE
      MessageInterface::ShowMessage
         ("   *** There is not enough data to interpolate %f, number of "
          "required points is %d, received %d points\n", ind, requiredPoints,
          pointCount);
      MessageInterface::ShowMessage
         ("Lagrange::IsInterpolationFeasible() returning -1\n");
      #endif
      return -1;
      #endif
   }
   
   SetRange();
   
   #ifdef DEBUG_LAGRANGE_FEASIBLE
   MessageInterface::ShowMessage
      ("   range1 = %f, range2 = %f\n", range[0], range[1]);
   #endif
   
   if (ind < range[0] && allowExtrapolation == false)
   {
      // Just return error code so that caller can handle it (LOJ: 2012.12.12)
      #if 0
      InterpolatorException ie;
      ie.SetDetails
         ("The requested data %f is not within the data range of [%f : %f]",
          ind, range[0],  range[1]);
      throw ie;
      #else
      #ifdef DEBUG_LAGRANGE_FEASIBLE
      MessageInterface::ShowMessage
         ("   *** The requested data %f is not within the data range of "
          "[%f : %f]. It is before the first data\n", ind, range[0],  range[1]);
      MessageInterface::ShowMessage
         ("Lagrange::IsInterpolationFeasible() returning -2\n");
      #endif
      return -2;
      #endif
   }
   
   // If independent data not within the range, throw an exception
   if (ind > range[1] && allowExtrapolation == false)
   {
      // Just return error code so that caller can handle it (LOJ: 2012.12.12)
      #if 0
      InterpolatorException ie;
      ie.SetDetails
         ("The requested data %f is not within the data range of [%f : %f]",
          ind, range[0],  range[1]);
      throw ie;
      #else
      #ifdef DEBUG_LAGRANGE_FEASIBLE
      MessageInterface::ShowMessage
         ("   *** The requested data %f is not within the data range of "
          "[%f : %f]. It is after the last data\n", ind, range[0],  range[1]);
      MessageInterface::ShowMessage
         ("Lagrange::IsInterpolationFeasible() returning -3\n");
      #endif
      return -3;
      #endif
   }
   
   #ifdef DEBUG_LAGRANGE_FEASIBLE
   MessageInterface::ShowMessage
      ("Lagrange::IsInterpolationFeasible() returning 1\n");
   #endif
   
   return 1;
}


//------------------------------------------------------------------------------
//  void Clear()
//------------------------------------------------------------------------------
/**
 * @see Interpolator
 */
//------------------------------------------------------------------------------
void LagrangeInterpolator::Clear()
{
   Interpolator::Clear();
   previousX = -9.9999e75;
   actualSize = 0;
   beginIndex = 0;
   startPoint = 0;
   
   for (Integer i = 0; i <= bufferSize; ++i)
      x[i] = -9.9999e75;
}


//------------------------------------------------------------------------------
//  bool AddPoint(const Real ind, const Real *data)
//------------------------------------------------------------------------------
/**
 * See Interpolator
 *
 * @exception thrown when independent data direction changes
 */
//------------------------------------------------------------------------------
bool LagrangeInterpolator::AddPoint(const Real ind, const Real *data)
{
   #ifdef DEBUG_LAGRANGE_ADD
   MessageInterface::ShowMessage
      ("Lagrange::AddPoint() entered, ind=%f, previousX=%g, data[0]=%f, "
       "data[1]=%f, data[2]=%f\n", ind, previousX, data[0], data[1], data[2]);
   #endif
   
   // To support backward propagation (LOJ: 2014.02.27)
   #if 0
   if (ind < previousX)
   {
      InterpolatorException ie;
      ie.SetDetails("The independent data provided is not monotonic, current data is %f, "
                    "previous data is %f", ind, previousX);
      throw ie;
   }
   #endif
   
   #ifdef DEBUG_LAGRANGE_ADD
   MessageInterface::ShowMessage
      ("Lagrange::AddPoint() returning Interpolator::AddPoint(ind, data)\n");
   #endif
   return Interpolator::AddPoint(ind, data);
}


//------------------------------------------------------------------------------
//  bool Interpolate(const Real ind, Real *results)
//------------------------------------------------------------------------------
/**
 * Perform the interpolation.
 * 
 * This method is the core interface for the lagrange interpolation.
 * See the GMAT math spec for the algorithm.
 * 
 * @param ind       The value of the independent parameter.
 * @param results   Data structure for the estimates.
 * 
 * @return true on success, false on failure.
 */
//------------------------------------------------------------------------------
bool LagrangeInterpolator::Interpolate(const Real ind, Real *results)
{
   #ifdef DUMP_DATA_POINT_20
      static bool dataDumped = false;
      static Integer calls = 0;
   #endif

   #ifdef DEBUG_LAGRANGE_INTERPOLATE
   MessageInterface::ShowMessage
      ("\nLagrange::Interpolate() entered, ind=%f, dimension=%d, forceInterpolation=%d\n",
       ind, dimension, forceInterpolation);
   #endif
   
   // Check for interpolation feasibility
   if (IsInterpolationFeasible(ind) != 1)
   {
      #ifdef DEBUG_LAGRANGE_INTERPOLATE
      MessageInterface::ShowMessage
         ("Lagrange::Interpolate() returnnig false, NOT feasible to interpolate at %f\n",
          ind);
      #endif
      return false;
   }
   
   // Build data points
   BuildDataPoints(ind);
   
   // Update index and check if it is inside a range
   if (!UpdateBeginAndEndIndex(ind))
   {
      // Check if not forcing interpolation (GMT-4110 fix)
      if (!forceInterpolation)
      {
         #ifdef DEBUG_LAGRANGE_INTERPOLATE
         MessageInterface::ShowMessage
            ("Lagrange::Interpolate() returnnig false, end index (%d) is out of "
             "bounds, beginIndex=%d\n", endIndex, beginIndex);
         #endif
         return false;
      }
   }
   
   // If not forcing interpolation, perform more checking
   if (!forceInterpolation)
   {
      if (!IsDataNearCenter(ind))
      {
         #ifdef DEBUG_LAGRANGE_INTERPOLATE
         MessageInterface::ShowMessage
            ("Lagrange::Interpolate() returning false, %f is NOT near center\n",
             ind);
         #endif
         return false;
      }
   }
   
   // Find starting point that will put ind in the center
   FindStartingPoint(ind);
   
   // Now interpolate using the algorithm in the Math Spec.
   Real *products = new Real[dimension];
   Real *estimates = new Real[dimension];
   
   #ifdef DUMP_DATA_POINT_20
      if (!dataDumped)
      {
         // Report from the 20th interpolation
         if (calls == 20)
         {
            MessageInterface::ShowMessage("\n\n==================================================\n");
            MessageInterface::ShowMessage("Lagrange data:\n\nDesired Epoch: %.12lf "
                                          "(Mod Julian %.12lf)\n\nData Points:\n\n", ind, ind / 86400.0); 
            for (Integer i = startPoint; i <= startPoint + order; ++i)
            {
               MessageInterface::ShowMessage("   %.12lf   ", x[i]);
               for (Integer dim = 0; dim < dimension; dim++)
                  MessageInterface::ShowMessage("   %.12lf", y[i][dim]);
               MessageInterface::ShowMessage("\n");
            }
            MessageInterface::ShowMessage("\n");
         }
      }
   #endif
   
   for (Integer dim = 0; dim < dimension; ++dim)
      estimates[dim] = 0.0;
   
   Integer endPoint = startPoint + order;
   #ifdef DEBUG_LAGRANGE_INTERPOLATE
   MessageInterface::ShowMessage
      ("   endPoint(%d) is computed from startPoint(%d) + order(%d)\n",
       endPoint, startPoint, order);
   #endif
   if (endPoint > actualSize - 1)
   {
      #ifdef DEBUG_LAGRANGE_INTERPOLATE
      MessageInterface::ShowMessage
         ("   endPoint(%d) is greater than lastIndex(%d), so decrementing start and end points\n",
          endPoint, actualSize-1);
      #endif
      --startPoint;
      --endPoint;
      for (Integer i = endPoint; i >= startPoint; --i)
      {
         if (x[i] == -9.9999e75)
            --endPoint;
      }
   }
   
   #ifdef DEBUG_LAGRANGE_INTERPOLATE
   MessageInterface::ShowMessage("   new startPoint=%d, endPoint=%d\n", startPoint, endPoint);
   #endif
   for (Integer i = startPoint; i <= endPoint; i++)
   {
      #ifdef DEBUG_LAGRANGE_INTERPOLATE
      MessageInterface::ShowMessage
         ("***** x[%d] = %.15f, y[%d] = %.15f, %.15f, %.15f\n", i, x[i], i, y[i][0], y[i][1], y[i][2]);
      #endif
      for (Integer dim = 0; dim < dimension; dim++)
      {
         products[dim] = y[i][dim];
         
         #ifdef DEBUG_LAGRANGE_INTERPOLATE_MORE
         MessageInterface::ShowMessage
            ("  i=%d, products[%d]=%f\n", i, dim, products[dim]);
         #endif
      }
      
      for (Integer j = startPoint; j <= endPoint; j++)
      {
         if (i != j)
         {
            for (Integer dim = 0; dim < dimension; ++dim)
            {
               // Should I check for divide by zero?
               if ((x[i] - x[j] == 0.0))
               {
                  MessageInterface::ShowMessage("WARNING: Lagrange interpolation zero denominator\n");
                  #ifdef DEBUG_LAGRANGE_INTERPOLATE_MORE
                  MessageInterface::ShowMessage("   x[%d]- x[%d]=%f\n", i, j, (x[i]- x[j]));
                  #endif
               }
               products[dim] = products[dim] * (ind - x[j]) / (x[i]- x[j]);
            }
         }
      }

      #ifdef DUMP_DATA_POINT_20
         if (!dataDumped)
         {
            // Report from the 20th interpolation
            if (calls == 20)
            {
               MessageInterface::ShowMessage("Products for %d:  ", i-startPoint); 
               for (Integer dim = 0; dim < dimension; dim++)
                  MessageInterface::ShowMessage("   %.12lf", products[dim]);
               MessageInterface::ShowMessage("\n");
            }
         }
      #endif

      #ifdef DEBUG_LAGRANGE_INTERPOLATE
      MessageInterface::ShowMessage
         ("***** products[%d] = %.15f, %.15f, %.15f\n", i, products[0], products[1], products[2]);
      #endif
      for (Integer dim = 0; dim < dimension; ++dim)
      {
         #ifdef DEBUG_LAGRANGE_INTERPOLATE_MORE
         MessageInterface::ShowMessage
            ("  i=%d, products[%d]=%f\n", i, dim, products[dim]);
         #endif
         estimates[dim] = estimates[dim] + products[dim];
      }
   }

   #ifdef DUMP_DATA_POINT_20
      if (!dataDumped)
      {
         // Report from the 20th interpolation
         if (calls == 20)
         {
            MessageInterface::ShowMessage("\nFinal estimate:  "); 
            for (Integer dim = 0; dim < dimension; dim++)
               MessageInterface::ShowMessage("   %.12lf", estimates[dim]);
            MessageInterface::ShowMessage("\n==================================================\n");
         }
      }
   #endif
   
   // copy results
   for (Integer dim = 0; dim < dimension; ++dim)
   {
      results[dim] = estimates[dim];
   }
   
   delete [] products;
   delete [] estimates;
   
   #ifdef DEBUG_LAGRANGE_INTERPOLATE
   MessageInterface::ShowMessage
      ("Lagrange::Interpolate() returning true, results[0:2] = %f, %f, %f\n",
       results[0], results[1], results[2]);
   #endif

   #ifdef DUMP_DATA_POINT_20
      if (!dataDumped)
      {
         if (calls == 20)
         {
            dataDumped = true;
         }
         ++calls;
      }
   #endif
   
   return true;
}


//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the LagrangeInterpolator.
 *
 * @return clone of the LagrangeInterpolator.
 */
//------------------------------------------------------------------------------
Interpolator* LagrangeInterpolator::Clone() const
{
   return (new LagrangeInterpolator(*this));
}


//---------------------------------
//  protected methods
//---------------------------------

//------------------------------------------------------------------------------
//  void AllocateArrays()
//------------------------------------------------------------------------------
/**
 * Allocates lagrange buffers and calls the base method to build the ring buffer.  
 */
//------------------------------------------------------------------------------
void LagrangeInterpolator::AllocateArrays()
{
   Interpolator::AllocateArrays();
   
   x = new Real[bufferSize+1];
   y = new Real*[bufferSize+1];

   for (Integer i = 0; i <= bufferSize; ++i)
   {
      x[i] = -9.9999e75;
      y[i]  = new Real[dimension];
   }
   
   latestPoint = -1;
}


//------------------------------------------------------------------------------
//  void CleanupArrays()
//------------------------------------------------------------------------------
/**
 * Frees the memory used by the lagrange buffer and calls the base method to 
 * manage the ring buffer.
 */
//------------------------------------------------------------------------------
void LagrangeInterpolator::CleanupArrays()
{
   if (x != NULL)
   {
      for (Integer i = 0; i <= bufferSize; ++i)
         delete [] y[i];
      delete [] x;
      delete [] y;

      x = NULL;
      y = NULL;
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
void LagrangeInterpolator::CopyArrays(const LagrangeInterpolator &i)
{
   Interpolator::CopyArrays(i);
   
   if (x == NULL)
      AllocateArrays();

   for (Integer j = 0; j <= bufferSize; ++j)
   {
      x[j] = i.x[j];
      memcpy( y[j],  i.y[j], dimension*sizeof(Real));
   }
}


//------------------------------------------------------------------------------
//  void BuildDataPoints(Real ind)
//------------------------------------------------------------------------------
/**
 * Use the ring buffer to load the arrays used to build the lagrange buffer.
 * Lagrange buffer should be constructed so that requested data sits in the
 * middle of the buffer as possible.
 */
//------------------------------------------------------------------------------
void LagrangeInterpolator::BuildDataPoints(Real ind)
{
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage("Lagrange::BuildDataPoints() entered, ind=%f\n", ind);
   #endif
   
   Integer i, j, start = 0;
   Real sign = (dataIncreases ? 1.0 : -1.0);
   Real indData = sign * independent[0];
   
   // Compute actual size to use since bufferSize is multiple size of order
   actualSize = bufferSize;
   if (actualSize > pointCount)
      actualSize = pointCount;
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("   indData=%f, bufferSize=%d, pointCount=%d, actualSize=%d\n",
       indData, bufferSize, pointCount, actualSize);
   #endif
   
   for (i = 1; i < actualSize; ++i)
   {
      if (sign*independent[i] < indData)
      {
         start = i;
         indData = sign*independent[i];
      }
   }
   
   for (i = 0; i < actualSize; ++i, ++start)
   {
      if (start == bufferSize)
         start = 0;
      x[i] = independent[start];
      
      for (j = 0; j < dimension; j++)
         y[i][j] = dependent[start][j];
      
      #ifdef DEBUG_LAGRANGE_BUILD
      MessageInterface::ShowMessage
         ("   start = %2d, x[%2d] = %.7f, y[%2d][0] = %f, y[%2d][2] = %f\n",
          start, i, x[i], i, y[i][0], i, y[i][2]);
      #endif
   }
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("Lagrange::BuildDataPoints() leaving\n");
   #endif
}


//------------------------------------------------------------------------------
// bool UpdateBeginAndEndIndex(Real ind)
//------------------------------------------------------------------------------
/**
 * Finds starting and ending index to use for interpolation.
 *
 * @return  true if indices are valid, false otherwise
 */
//------------------------------------------------------------------------------
bool LagrangeInterpolator::UpdateBeginAndEndIndex(Real ind)
{
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("Lagrange::UpdateBeginAndEndIndex() entered, ind=%f, beginIndex=%d, "
       "endIndex=%d, actualSize=%d\n", ind, beginIndex, endIndex, actualSize);
   #endif
   
   Integer nearestDataIndex = 0;
   bool isIndexValid = true;
   
   for (Integer i = 0; i < actualSize; i++)
   {
      if (x[i] >= ind)
      {
         nearestDataIndex = i;
         break;
      }
   }
   
   nearestDataIndex = nearestDataIndex - 1;
   Integer begin = nearestDataIndex - (order/2);
   Integer end = nearestDataIndex + (order/2);
   
   // Update beginIndex here
   beginIndex = begin;
   endIndex = end;
   dataIndex = nearestDataIndex;
   
   if ((endIndex - beginIndex) < order)
   {
      if (beginIndex == 0)
      {
         if (fabs(x[beginIndex] - ind) > fabs(x[beginIndex+1] - ind))
         {
            beginIndex = begin + 1;
            endIndex = end + 1;
         }
         else
            endIndex = end + 1;
      }
      else if (beginIndex > 0)
      {
         if (fabs(x[beginIndex] - ind) > fabs(x[beginIndex-1] - ind))
            beginIndex = begin - 1;
         else
            endIndex = end + 1;
      }
   }
   
   if (endIndex >= actualSize)
   {
      endIndex = actualSize - 1;
      beginIndex = endIndex - order;
      isIndexValid = false;
   }
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("   dataIndex=%d, begin=%d, end=%d, beginIndex=%d, endIndex=%d\n",
       dataIndex, begin, end, beginIndex, endIndex);
   MessageInterface::ShowMessage
      ("Lagrange::UpdateBeginAndEndIndex() returning %d\n", isIndexValid);
   #endif
   
   return isIndexValid;
}


//------------------------------------------------------------------------------
// bool IsDataNearCenter(Real ind)
//------------------------------------------------------------------------------
/**
 * Checks if requested data is centered in the buffer.
 */
//------------------------------------------------------------------------------
bool LagrangeInterpolator::IsDataNearCenter(Real ind)
{
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("Lagrange::IsDataNearCenter() entered, ind=%f, beginIndex=%d, endIndex=%d, actualSize=%d\n",
       ind, beginIndex, endIndex, actualSize);
   #endif
   
   bool retval = false;
   // Check if there are enough points before and after data requested
   //if (beginIndex >= 0 && endIndex < actualSize)
   //   retval = true;
   
   // Check if dataIndex is in the middle in the buffer
   if (beginIndex >= 0 && endIndex < actualSize)
   {
      if (dataIndex + ((order+1)/2) < actualSize)
         retval = true;
   }
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("Lagrange::IsDataNearCenter() returning %d\n", retval);
   #endif
   
   return retval;
}


//------------------------------------------------------------------------------
// Integer FindStartingPoint(Real ind)
//------------------------------------------------------------------------------
/*
 * Finds index that places ind in the center of the interpolation range using
 * algorithm given in the math spec.
 */
//------------------------------------------------------------------------------
Integer LagrangeInterpolator::FindStartingPoint(Real ind)
{
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage("Lagrange::FindStartingPoint() entered, ind=%f\n", ind);
   #endif
   
   // Find the data point near the requested value
   Real minDiff = 1.0e30, diff, meanX;
   Integer qMin = 0;
   Integer qEnd = beginIndex + order;
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage("   beginIndex=%d, qEnd=%d\n", beginIndex, qEnd);
   #endif
   
   // LOJ: use q <= qEnd (2013.07.31)
   for (Integer q = beginIndex; q <= qEnd; ++q)
   {
      meanX = ( x[q + order] + x[q] ) / 2;
      diff = GmatMathUtil::Abs( meanX - ind );
      if (diff < minDiff)
      {
         qMin = q;
         minDiff = diff;
      }
   }
   
   // Assign to starting point
   startPoint = qMin;
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage("   startPoint set to qMin (%d)\n", qMin);
   #endif
   
   // We don't want start point to pass the actual data size, so adjust
   //if (qMin + order > actualSize - 1)
   if (qMin + requiredPoints > actualSize - 1)
   {
      startPoint = actualSize - order;
      #ifdef DEBUG_LAGRANGE_BUILD
      MessageInterface::ShowMessage
         ("   qMin + requiredPoints = %d passed the last index %d, so adjusted "
          "startPoint to %d\n", qMin+requiredPoints, actualSize-1, startPoint);
      #endif
   }
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("   before adjusting startPoint, beginIndex=%d, qEnd=%d, qMin=%d, startPoint=%d\n",
       beginIndex, qEnd, qMin, startPoint);
   #endif
   
   if (startPoint < 0)
      startPoint = 0;
   
   if (beginIndex > 0)
      startPoint = beginIndex;
   
   #ifdef DEBUG_LAGRANGE_BUILD
   MessageInterface::ShowMessage
      ("Lagrange::FindStartingPoint() returning startPoint = %d\n",
       startPoint);
   #endif
   
   return startPoint;
}


