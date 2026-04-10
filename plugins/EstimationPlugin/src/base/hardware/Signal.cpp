//$Id: Sensor.cpp 67 2010-06-08 21:56:16Z  $
//------------------------------------------------------------------------------
//                         Sensor
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
// Author: Tuan Dang Nguyen
// Created: June 8, 2010
//
/**
 * The signal of a sensor.
 */
//------------------------------------------------------------------------------
#include "Signal.hpp"

//------------------------------------------------------------------------------
// Signal()
//------------------------------------------------------------------------------
/**
 * Constructor
 */
//------------------------------------------------------------------------------
Signal::Signal()
{
   epoch = 0;
   value = 0;
}

//------------------------------------------------------------------------------
// ~Signal()
//------------------------------------------------------------------------------
/**
 * Destructor
 */
//------------------------------------------------------------------------------
Signal::~Signal()
{
}


//-----------------------------------------------------------------------------
// Signal(const Signal & sig)
//-----------------------------------------------------------------------------
/**
 * Copy constructor
 *
 * @param sig The signal that is copied into this new instance.
 */
//-----------------------------------------------------------------------------
Signal::Signal(const Signal & sig) :
   epoch      (sig.epoch),
   value      (sig.value)
{
}

//-----------------------------------------------------------------------------
// Signal & operator=(const Signal & sig)
//-----------------------------------------------------------------------------
/**
 * Assignment operator.
 *
 * @param sig The signal that provides the configuration for this
 * instance.
 *
 * @return This object, configured to look like sig.
 */
//-----------------------------------------------------------------------------
Signal & Signal::operator=(const Signal & sig)
{
   if (this != &sig)
   {
      epoch = sig.epoch;
      value = sig.value;
   }

   return *this;
}



////------------------------------------------------------------------------------
//// void SetEpoch(GmatEpoch ep)
////------------------------------------------------------------------------------
///**
// * Set epoch for the signal
// */
////------------------------------------------------------------------------------
//void Signal::SetEpoch(GmatEpoch ep)
//{
//   epoch = ep;
//}


//------------------------------------------------------------------------------
// void SetEpoch(GmatTime ep)
//------------------------------------------------------------------------------
/**
* Set epoch for the signal
*/
//------------------------------------------------------------------------------
void Signal::SetEpoch(GmatTime ep)
{
   epoch = ep;
}


////------------------------------------------------------------------------------
//// GmatEpoch GetEpoch()
////------------------------------------------------------------------------------
///**
// * Get epoch of the signal
// */
////------------------------------------------------------------------------------
//GmatEpoch Signal::GetEpoch()
//{
//   return epoch;
//}


//------------------------------------------------------------------------------
// GmatTime GetEpoch()
//------------------------------------------------------------------------------
/**
 * Get epoch of the signal
 */
 //------------------------------------------------------------------------------
GmatTime Signal::GetEpoch()
{
   return epoch;
}


//------------------------------------------------------------------------------
// bool SetValue(Real v)
//------------------------------------------------------------------------------
/**
 * Set frequency to the signal
 */
//------------------------------------------------------------------------------
bool Signal::SetValue(Real v)
{
   value = v;
   return true;
}


//------------------------------------------------------------------------------
// Real GetValue(Real v)
//------------------------------------------------------------------------------
/**
 * Get frequency of the signal
 */
//------------------------------------------------------------------------------
Real Signal::GetValue()
{
   return value;
}
