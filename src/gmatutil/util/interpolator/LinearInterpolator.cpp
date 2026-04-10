//$Id$
//------------------------------------------------------------------------------
//                             LinearInterpolator
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
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
// Author: Darrel J. Conway
// Created: 2003/10/15
//
// Developed jointly by NASA/GSFC and Thinking Systems, Inc. under contract
// number S-67573-G
//
/**
 * Implementation of the linear interpolator.
 */
//------------------------------------------------------------------------------


#include "LinearInterpolator.hpp" // class's header file

//------------------------------------------------------------------------------
//  LinearInterpolator(const std::string &name, Integer dim)
//------------------------------------------------------------------------------
/**
 * Creates an instance of a LinearInterpolator (default constructor).
 * 
 * @param <name> Name for this interpolator.
 * @param <dim>  Dimension of data that will be interpolated (defaults to 1).
 */
//------------------------------------------------------------------------------
LinearInterpolator::LinearInterpolator(const std::string &name, Integer dim) :
    Interpolator        (name, "LinearInterpolator", dim)
{
}


//------------------------------------------------------------------------------
//  ~LinearInterpolator()
//------------------------------------------------------------------------------
/**
 * Removes instance of a LinearInterpolator from memory (destructor).
 */
//------------------------------------------------------------------------------
LinearInterpolator::~LinearInterpolator()
{
}


//------------------------------------------------------------------------------
//  LinearInterpolator(const LinearInterpolator &li)
//------------------------------------------------------------------------------
/**
 * Creates a copy of a LinearInterpolator based on another (copy constructor).
 * 
 * @param li    The original that is copied.
 */
//------------------------------------------------------------------------------
LinearInterpolator::LinearInterpolator(const LinearInterpolator &li) :
    Interpolator        (li)
{
}


//------------------------------------------------------------------------------
//  LinearInterpolator& operator=(const LinearInterpolator &li)
//------------------------------------------------------------------------------
/**
 * Sets this LinearInterpolator to match another (assignment operator).
 * 
 * @param li    The original that is copied.
 */
//------------------------------------------------------------------------------
LinearInterpolator& LinearInterpolator::operator=(const LinearInterpolator &li)
{
    if (&li == this)
        return *this;
        
    Interpolator::operator=(li);
    return *this;
}


//------------------------------------------------------------------------------
//  bool Interpolate(const Real ind, Real *results)
//------------------------------------------------------------------------------
/**
 * Finds the interpolated value based on data fed in prior to this call.
 * 
 * @param ind       The value of the independent parameter.
 * @param results   Array that will contain the results of the interpolation.
 * 
 * @return true if the interpolation succeeds, false if it fails.
 */
//------------------------------------------------------------------------------
bool LinearInterpolator::Interpolate(const Real ind, Real *results)
{
    if (pointCount < requiredPoints)
       throw InterpolatorException("ERROR - LinearInterpolator: " +
          GmatStringUtil::ToString(requiredPoints, 1) + " points "
          "are required for interpolation, but only " +
          GmatStringUtil::ToString(pointCount, 1) + " were provided.\n");
        
    Integer index         = latestPoint;
    Integer previousPoint = latestPoint-1;
    Integer valid         = pointCount;
    Integer i;
    Real delta;
    
    while ((previousPoint != latestPoint) && valid > 0) {
        if (previousPoint < 0)
            previousPoint += bufferSize;
        if (index < 0)
            index += bufferSize;
        // Check to see that ind is in the valid range
        SetRange();
        if (((ind < range[0]) || (ind > range[1])) && allowExtrapolation == false)
            return false;
            
        if (ind >= independent[previousPoint] || (allowExtrapolation && ind < independent[previousPoint] && previousPoint == 0))
        {
            // Found the bounding points
            delta = independent[index] - independent[previousPoint];
            if (delta == 0.0)
                return false;
            delta = (ind - independent[previousPoint]) / delta;
            for (i = 0; i < dimension; ++i)
                results[i] = dependent[previousPoint][i] + delta *
                             (dependent[index][i]-dependent[previousPoint][i]);
                             
            return true;
        }
        
        index = previousPoint;
        --previousPoint;
        --valid;
    }

    return false;
}

//------------------------------------------------------------------------------
//  GmatBase* Clone() const
//------------------------------------------------------------------------------
/**
 * This method returns a clone of the LinearInterpolator.
 *
 * @return clone of the LinearInterpolator.
 */
//------------------------------------------------------------------------------
Interpolator* LinearInterpolator::Clone() const
{
   return (new LinearInterpolator(*this));
}

