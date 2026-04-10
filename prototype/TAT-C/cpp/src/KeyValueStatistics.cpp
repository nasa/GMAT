//------------------------------------------------------------------------------
//                           KeyValueStatistics
//------------------------------------------------------------------------------
// GMAT: General Mission Analysis Tool.
//
// Copyright (c) 2002 - 2017 United States Government as represented by the
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
// Author: Wendy Shoan, NASA/GSFC
// Created: 2016.05.02
//
/**
 * Implementation of KeyValueStatistics class.
 */
//------------------------------------------------------------------------------

#include "gmatdefs.hpp"
#include "KeyValueStatistics.hpp"

//------------------------------------------------------------------------------
// static data
//------------------------------------------------------------------------------
// None

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// KeyValueStatistics()
//------------------------------------------------------------------------------
/**
 * Default constructor.
 *
 */
//------------------------------------------------------------------------------
KeyValueStatistics::KeyValueStatistics(Real minVal, Real maxVal, Real avgVal) :
   minValue (minVal),
   maxValue (maxVal),
   avgValue (avgVal)
{
}

//------------------------------------------------------------------------------
// KeyValueStatistics(const KeyValueStatistics &copy)
//------------------------------------------------------------------------------
/**
 * Copy constructor.
 * 
 * @param copy  the object to copy
 *
 */
//------------------------------------------------------------------------------
KeyValueStatistics::KeyValueStatistics(const KeyValueStatistics &copy) :
   minValue   (copy.minValue),
   maxValue   (copy.maxValue),
   avgValue   (copy.avgValue)
{
}

//------------------------------------------------------------------------------
// KeyValueStatistics& operator=(const KeyValueStatistics &copy)
//------------------------------------------------------------------------------
/**
 * The operator= for the KeyValueStatistics class.
 * 
 * @param copy  the object to copy
 *
 */
//------------------------------------------------------------------------------
KeyValueStatistics& KeyValueStatistics::operator=(
                                        const KeyValueStatistics &copy)
{
   if (&copy == this)
      return *this;
   
   minValue    = copy.minValue;
   maxValue    = copy.maxValue;
   avgValue    = copy.avgValue;
   
   return *this;
}

//------------------------------------------------------------------------------
// ~KeyValueStatistics()
//------------------------------------------------------------------------------
/**
 * Destructor.
 *
 */
//------------------------------------------------------------------------------
KeyValueStatistics::~KeyValueStatistics()
{
}

//------------------------------------------------------------------------------
//  Real GetMinValue()
//------------------------------------------------------------------------------
/**
 * Returns the minimum value.
 * 
 * @return   the minimum value
 *
 */
//------------------------------------------------------------------------------
Real KeyValueStatistics::GetMinValue()
{
   return minValue;
}

//------------------------------------------------------------------------------
//  Real GetMaxValue()
//------------------------------------------------------------------------------
/**
 * Returns the maximum value.
 * 
 * @return   the maximum value
 *
 */
//------------------------------------------------------------------------------
Real KeyValueStatistics::GetMaxValue()
{
   return maxValue;
}

//------------------------------------------------------------------------------
//  Real GetAvgValue()
//------------------------------------------------------------------------------
/**
 * Returns the average value.
 * 
 * @return   the average value
 *
 */
//------------------------------------------------------------------------------
Real KeyValueStatistics::GetAvgValue()
{
   return avgValue;
}


//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// None
