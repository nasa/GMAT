//------------------------------------------------------------------------------
//                              BoundData
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
// Author: Wendy Shoan / NASA
// Created: 2016.01.05
//
/**
 * From original MATLAB prototype:
 *  Author: S. Hughes.  steven.p.hughes@nasa.gov
 */
//------------------------------------------------------------------------------

#include "BoundData.hpp"

//------------------------------------------------------------------------------
// public methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// default constructor
//------------------------------------------------------------------------------
BoundData::BoundData()
{
   emptyBoundVector.SetSize(1);
   emptyBoundVector(0) = 0.0;
}

//------------------------------------------------------------------------------
// copy constructor
//------------------------------------------------------------------------------
BoundData::BoundData(const BoundData &copy)
{
   if (copy.stateLowerBound.IsSized())
      stateLowerBound = copy.stateLowerBound;
   if (copy.stateUpperBound.IsSized())
      stateUpperBound = copy.stateUpperBound;
   if (copy.controlLowerBound.IsSized())
      controlLowerBound = copy.controlLowerBound;
   if (copy.controlUpperBound.IsSized())
      controlUpperBound = copy.controlUpperBound;
   if (copy.timeLowerBound.IsSized())
      timeLowerBound = copy.timeLowerBound;
   if (copy.timeUpperBound.IsSized())
      timeUpperBound = copy.timeUpperBound;
   if (copy.staticLowerBound.IsSized())
      staticLowerBound = copy.staticLowerBound;
   if (copy.staticUpperBound.IsSized())
      staticUpperBound = copy.staticUpperBound;
   if (copy.emptyBoundVector.IsSized())
      emptyBoundVector = copy.emptyBoundVector;
}


//------------------------------------------------------------------------------
// operator=
//------------------------------------------------------------------------------
BoundData& BoundData::operator=(const BoundData &copy)
{
   
   if (&copy == this)
      return *this;

   if (copy.stateLowerBound.IsSized())
      stateLowerBound = copy.stateLowerBound;
   if (copy.stateUpperBound.IsSized())
      stateUpperBound = copy.stateUpperBound;
   if (copy.controlLowerBound.IsSized())
      controlLowerBound = copy.controlLowerBound;
   if (copy.controlUpperBound.IsSized())
      controlUpperBound = copy.controlUpperBound;
   if (copy.timeLowerBound.IsSized())
      timeLowerBound = copy.timeLowerBound;
   if (copy.timeUpperBound.IsSized())
      timeUpperBound = copy.timeUpperBound;
   if (copy.staticLowerBound.IsSized())
      staticLowerBound = copy.staticLowerBound;
   if (copy.staticUpperBound.IsSized())
      staticUpperBound = copy.staticUpperBound;
   if (copy.emptyBoundVector.IsSized())
      emptyBoundVector = copy.emptyBoundVector;

   return *this;
   
}

//------------------------------------------------------------------------------
// destructor
//------------------------------------------------------------------------------
BoundData::~BoundData()
{
   // nothing to do here
}



//------------------------------------------------------------------------------
// Set methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  void SetStateLowerBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
 * This method sets the state lower bound.
 *
 * @param <bound> the input state lower bound value
 */
//------------------------------------------------------------------------------
void BoundData::SetStateLowerBound(const Rvector &bound)
{
   if (bound.IsSized())
      stateLowerBound = bound;
}

//------------------------------------------------------------------------------
//  void SetStateUpperBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
 * This method sets the state upper bound.
 *
 * @param <bound> the input state upper bound value
 */
//------------------------------------------------------------------------------
void BoundData::SetStateUpperBound(const Rvector &bound)
{
   if (bound.IsSized())
      stateUpperBound = bound;
}

//------------------------------------------------------------------------------
//  void SetControlLowerBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
 * This method sets the control lower bound.
 *
 * @param <bound> the input control lower bound value
 */
//------------------------------------------------------------------------------
void BoundData::SetControlLowerBound(const Rvector &bound)
{
   if (bound.IsSized())
      controlLowerBound = bound;
}

//------------------------------------------------------------------------------
//  void SetControlUpperBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
 * This method sets the control upper bound.
 *
 * @param <bound> the input control upper bound value
 */
//------------------------------------------------------------------------------
void BoundData::SetControlUpperBound(const Rvector &bound)
{
   if (bound.IsSized())
      controlUpperBound = bound;
}

//------------------------------------------------------------------------------
//  void SetTimeLowerBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
 * This method sets the time lower bound.
 *
 * @param <bound> the input time lower bound value
 */
//------------------------------------------------------------------------------
void BoundData::SetTimeLowerBound(const Rvector &bound)
{
   if (bound.IsSized())
      timeLowerBound = bound;
}

//------------------------------------------------------------------------------
//  void SetTimeUpperBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
 * This method sets the time upper bound.
 *
 * @param <bound> the input time upper bound value
 */
//------------------------------------------------------------------------------
void BoundData::SetTimeUpperBound(const Rvector &bound)
{
   if (bound.IsSized())
      timeUpperBound = bound;
}


//------------------------------------------------------------------------------
//  void SetStaticLowerBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
* This method sets the static lower bound.
*
* @param <bound> the input static lower bound value
*/
//------------------------------------------------------------------------------
void BoundData::SetStaticLowerBound(const Rvector &bound)
{
   if (bound.IsSized())
      staticLowerBound = bound;
}

//------------------------------------------------------------------------------
//  void SetStaticUpperBound(const Rvector &bound)
//------------------------------------------------------------------------------
/**
* This method sets the static upper bound.
*
* @param <bound> the input static upper bound value
*/
//------------------------------------------------------------------------------
void BoundData::SetStaticUpperBound(const Rvector &bound)
{
   if (bound.IsSized())
      staticUpperBound = bound;
}

//------------------------------------------------------------------------------
// Get methods
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
//  const Rvector& GetStateLowerBound()
//------------------------------------------------------------------------------
/**
 * This method returns the state lower bound.
 *
 * @return the state lower bound vector
 */
//------------------------------------------------------------------------------
const Rvector& BoundData::GetStateLowerBound()
{
   return stateLowerBound;
}

//------------------------------------------------------------------------------
//  const Rvector& GetStateUpperBound()
//------------------------------------------------------------------------------
/**
 * This method returns the state upper bound.
 *
 * @return the state upper bound vector
 */
//------------------------------------------------------------------------------
const Rvector& BoundData::GetStateUpperBound()
{
   return stateUpperBound;
}

//------------------------------------------------------------------------------
//  const Rvector& GetControlLowerBound()
//------------------------------------------------------------------------------
/**
 * This method returns the control lower bound.
 *
 * @return the control lower bound vector
 */
//------------------------------------------------------------------------------
const Rvector& BoundData::GetControlLowerBound()
{
   if (controlLowerBound.IsSized() == true)
      return controlLowerBound;
   else
      return emptyBoundVector;
}

//------------------------------------------------------------------------------
//  const Rvector& GetControlUpperBound()
//------------------------------------------------------------------------------
/**
 * This method returns the control upper bound.
 *
 * @return the control upper bound vector
 */
//------------------------------------------------------------------------------
const Rvector& BoundData::GetControlUpperBound()
{
   if (controlUpperBound.IsSized() == true)
      return controlUpperBound;
   else
      return emptyBoundVector;
}

//------------------------------------------------------------------------------
//  const Rvector& GetTimeLowerBound()
//------------------------------------------------------------------------------
/**
 * This method returns the time lower bound.
 *
 * @return the time lower bound vector
 */
//------------------------------------------------------------------------------
const Rvector& BoundData::GetTimeLowerBound()
{
   return timeLowerBound;
}

//------------------------------------------------------------------------------
//  const Rvector& GetTimeUpperBound()
//------------------------------------------------------------------------------
/**
 * This method returns the time upper bound.
 *
 * @return the time upper bound vector
 */
//------------------------------------------------------------------------------
const Rvector& BoundData::GetTimeUpperBound()
{
   return timeUpperBound;
}

//------------------------------------------------------------------------------
//  const Rvector& GetStaticLowerBound()
//------------------------------------------------------------------------------
/**
* This method returns the static lower bound.
*
* @return the static lower bound vector
*/
//------------------------------------------------------------------------------
const Rvector& BoundData::GetStaticLowerBound()
{
   if (staticLowerBound.IsSized() == true)
      return staticLowerBound;
   else
      return emptyBoundVector;
}

//------------------------------------------------------------------------------
//  const Rvector& GetStaticUpperBound()
//------------------------------------------------------------------------------
/**
* This method returns the state upper bound.
*
* @return the state upper bound vector
*/
//------------------------------------------------------------------------------
const Rvector& BoundData::GetStaticUpperBound()
{
   if (staticUpperBound.IsSized() == true)
      return staticUpperBound;
   else
      return emptyBoundVector;

   return staticUpperBound;
}

//------------------------------------------------------------------------------
// protected methods
//------------------------------------------------------------------------------
// none
